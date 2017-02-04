#include "AssetManager.h"



AssetManager::AssetManager(Shader* InDefaultShader, vr::IVRSystem* InHMD) {
	DefaultShader = InDefaultShader;
	HMD = InHMD;
	SetupRenderModels();
}


AssetManager::~AssetManager() {
}


void AssetManager::SetupRenderModels() {
	memset(TrackedModel, 0, sizeof(TrackedModel));

	if (!HMD)
		return;

	for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++) {
		if (!HMD->IsTrackedDeviceConnected(unTrackedDevice))
			continue;

		SetupRenderModelForTrackedDevice(unTrackedDevice);
	}

}

void AssetManager::SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex) {
	if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
		return;

	// try to find a model we've already set up
	std::string sRenderModelName = GetTrackedDeviceString(HMD, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);
	Asset* Model = FindOrLoadRenderModel(sRenderModelName.c_str());
	if (!Model) {
		std::string sTrackingSystemName = GetTrackedDeviceString(HMD, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String);
		printf("Unable to load render model for tracked device %d (%s.%s)", unTrackedDeviceIndex, sTrackingSystemName.c_str(), sRenderModelName.c_str());
	} else {
		TrackedModel[unTrackedDeviceIndex] = Model;
		bShowTrackedDevice[unTrackedDeviceIndex] = true;
	}
}

bool* AssetManager::GetTrackedDevices() {
	return bShowTrackedDevice;
}

std::string AssetManager::GetTrackedDeviceString(vr::IVRSystem* pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError) {
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

Asset* AssetManager::FindOrLoadRenderModel(const char *pchRenderModelName) {
	Asset *pRenderModel = NULL;
	for (std::vector< Asset * >::iterator i = ViveModels.begin(); i != ViveModels.end(); i++) {
		if (!_stricmp((*i)->GetName().c_str(), pchRenderModelName)) {
			pRenderModel = *i;
			break;
		}
	}

	// load the model if we didn't find one
	if (!pRenderModel) {
		vr::RenderModel_t *pModel;
		vr::EVRRenderModelError error;
		while (1) {
			error = vr::VRRenderModels()->LoadRenderModel_Async(pchRenderModelName, &pModel);
			if (error != vr::VRRenderModelError_Loading)
				break;

			Sleep(1);
		}

		if (error != vr::VRRenderModelError_None) {
			printf("Unable to load render model %s - %s\n", pchRenderModelName, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(error));
			return NULL; // move on to the next tracked device
		}

		vr::RenderModel_TextureMap_t *pTexture;
		while (1) {
			error = vr::VRRenderModels()->LoadTexture_Async(pModel->diffuseTextureId, &pTexture);
			if (error != vr::VRRenderModelError_Loading)
				break;

			Sleep(1);
		}

		if (error != vr::VRRenderModelError_None) {
			printf("Unable to load render texture id:%d for render model %s\n", pModel->diffuseTextureId, pchRenderModelName);
			vr::VRRenderModels()->FreeRenderModel(pModel);
			return NULL; // move on to the next tracked device
		}

		pRenderModel = new Asset(pchRenderModelName);
		if (!pRenderModel->Init(*pModel, *pTexture)) {
			printf("Unable to create GL model from render model %s\n", pchRenderModelName);
			delete pRenderModel;
			pRenderModel = NULL;
		} else {
			ViveModels.push_back(pRenderModel);
		}
		vr::VRRenderModels()->FreeRenderModel(pModel);
		vr::VRRenderModels()->FreeTexture(pTexture);
	}
	return pRenderModel;
}
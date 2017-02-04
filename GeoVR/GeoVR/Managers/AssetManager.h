#pragma once
#include <openvr.h>
#include <cstdlib>
#include <stdio.h>
#include "ShaderManager.h"
#include "../Models/Asset.h"
#include <windows.h>

class AssetManager {
public:
	AssetManager(Shader* InDefaultShader, vr::IVRSystem* InHMD);
	~AssetManager();
	void SetupRenderModels();
	void SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);

	bool* GetTrackedDevices();

private:
	std::string GetTrackedDeviceString(vr::IVRSystem* pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError = NULL);
	Asset* FindOrLoadRenderModel(const char * pchRenderModelName);

	Shader* DefaultShader;
	vr::IVRSystem* HMD;

	std::vector<Asset*> ViveModels;
	Asset* TrackedModel[vr::k_unMaxTrackedDeviceCount];
	bool bShowTrackedDevice[vr::k_unMaxTrackedDeviceCount];
};


#include "Camera.h"

Camera::Camera(vr::IVRSystem* InHMD) {
	HMD = InHMD;
	NearClip = 0.1f;
	FarClip = 30.0f;

	Projection_Left = GetHMDMatrixProjectionEye(vr::Eye_Left);
	Projection_Right = GetHMDMatrixProjectionEye(vr::Eye_Right);
	WorldPosition_Left = GetHMDMatrixPoseEye(vr::Eye_Left);
	WorldPosition_Right = GetHMDMatrixPoseEye(vr::Eye_Right);
	SetupStereoRenderTargets();

	ValidPoseCount = 0;

}

Camera::~Camera() {
}

FramebufferDesc Camera::GetLeftFrame() {
	return LeftEyeFrame;
}

FramebufferDesc Camera::GetRightFrame() {
	return RightEyeFrame;
}

uint32_t Camera::GetFrameWidth() {
	return Render_X;
}

uint32_t Camera::GetFrameHeight() {
	return Render_Y;
}

Matrix4 Camera::GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye) {
	if (!HMD)
		return Matrix4();

	vr::HmdMatrix44_t mat = HMD->GetProjectionMatrix(nEye, NearClip, FarClip);

	return Matrix4(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
}

Matrix4 Camera::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye) {
	if (!HMD)
		return Matrix4();

	vr::HmdMatrix34_t matEyeRight = HMD->GetEyeToHeadTransform(nEye);
	Matrix4 matrixObj(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
	);

	return matrixObj.invert();
}

Matrix4 Camera::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye) {
	Matrix4 matMVP;
	if (nEye == vr::Eye_Left) {
		matMVP = Projection_Left * WorldPosition_Left * HMDPose;
	} else if (nEye == vr::Eye_Right) {
		matMVP = Projection_Right * WorldPosition_Right * HMDPose;
	}

	return matMVP;
}

bool Camera::SetupStereoRenderTargets() {
	HMD->GetRecommendedRenderTargetSize(&Render_X, &Render_Y);

	CreateFrameBuffer(Render_X, Render_Y, LeftEyeFrame);
	CreateFrameBuffer(Render_X, Render_Y, RightEyeFrame);

	return true;
}

bool Camera::CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc) {
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Camera::UpdateHMDMatrixPose() {
	if (!HMD)
		return;

	vr::VRCompositor()->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	ValidPoseCount = 0;
	std::string PoseClasses = "";
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
		if (TrackedDevicePose[nDevice].bPoseIsValid) {
			ValidPoseCount++;

			vr::HmdMatrix34_t matPose = TrackedDevicePose[nDevice].mDeviceToAbsoluteTracking;
			Matrix4 matrixObj(
				matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
				matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
				matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
				matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
			);
			DevicePose[nDevice] = matrixObj;

			if (DeviceClassChar[nDevice] == 0) {
				switch (HMD->GetTrackedDeviceClass(nDevice)) {
				case vr::TrackedDeviceClass_Controller:        DeviceClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               DeviceClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           DeviceClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_GenericTracker:    DeviceClassChar[nDevice] = 'G'; break;
				case vr::TrackedDeviceClass_TrackingReference: DeviceClassChar[nDevice] = 'T'; break;
				default:                                       DeviceClassChar[nDevice] = '?'; break;
				}
			}
			PoseClasses += DeviceClassChar[nDevice];
		}
	}

	if (TrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
		HMDPose = DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
		HMDPose.invert();
	}
}
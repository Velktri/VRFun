#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_main.h>

/* GLM */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shared\Vectors.h"
#include "Shared\Matrices.h"


#include <iostream>
#include "Models\Asset.h"
#include <openvr.h>

class Asset;

struct FramebufferDesc {
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

class Camera {
public:
	Camera(vr::IVRSystem* InHMD);
	~Camera();

	FramebufferDesc GetLeftFrame();
	FramebufferDesc GetRightFrame();
	uint32_t GetFrameWidth();
	uint32_t GetFrameHeight();
	Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
	Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
	Matrix4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
	void UpdateHMDMatrixPose();

private:
	vr::IVRSystem* HMD;

	Matrix4 Projection_Left;
	Matrix4 Projection_Right;
	Matrix4 WorldPosition_Left;
	Matrix4 WorldPosition_Right;


	float NearClip;
	float FarClip;

	/* Framebuffers */
	uint32_t Render_X;
	uint32_t Render_Y;

	FramebufferDesc LeftEyeFrame;
	FramebufferDesc RightEyeFrame;

	bool SetupStereoRenderTargets();
	bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);

	vr::TrackedDevicePose_t TrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	Matrix4 DevicePose[vr::k_unMaxTrackedDeviceCount];
	char DeviceClassChar[vr::k_unMaxTrackedDeviceCount];
	Matrix4 HMDPose;
	int ValidPoseCount;

};
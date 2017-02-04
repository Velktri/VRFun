#pragma once
#include <openvr.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include "World.h"
#include "Input.h"

class VREngine {
public:
	VREngine();
	~VREngine();

	bool Init();
	void Loop();
	void Shutdown();

private:
	vr::IVRSystem* HMD;
	int WindowPos_X;
	int WindowPos_Y;
	int WindowSize_X;
	int WindowSize_Y;
	SDL_Window* Window;
	SDL_GLContext Context;
	bool VSync;
	World* SceneWorld;
	Input* SceneInput;

	std::string GetTrackedDeviceString(vr::IVRSystem * pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError = NULL);
};


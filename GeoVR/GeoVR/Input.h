#pragma once
#include <SDL.h>
#include <openvr.h>
#include <stdio.h>
#include <string>
#include "Managers\AssetManager.h"

class Input {
public:
	Input(vr::IVRSystem* InHMD);
	~Input();

	bool HandleInput(AssetManager* InAM);



private:
	void ProcessVREvent(const vr::VREvent_t & event, AssetManager* InAM);
	vr::IVRSystem* HMD;
};


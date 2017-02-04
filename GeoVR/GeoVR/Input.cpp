#include "Input.h"



Input::Input(vr::IVRSystem* InHMD) {
	HMD = InHMD;
}


Input::~Input() {
}

bool Input::HandleInput(AssetManager* InAM) {
	SDL_Event sdlEvent;
	bool bRet = false;

	while (SDL_PollEvent(&sdlEvent) != 0) {
		if (sdlEvent.type == SDL_QUIT) {
			bRet = true;
		} else if (sdlEvent.type == SDL_KEYDOWN) {
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE
				|| sdlEvent.key.keysym.sym == SDLK_q) {
				bRet = true;
			}
			//if (sdlEvent.key.keysym.sym == SDLK_c) {
			//	m_bShowCubes = !m_bShowCubes;
			//}
		}
	}

	// Process SteamVR events
	vr::VREvent_t event;
	while (HMD->PollNextEvent(&event, sizeof(event))) {
		ProcessVREvent(event, InAM);
	}

	// Process SteamVR controller state
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++) {
		vr::VRControllerState_t state;
		if (HMD->GetControllerState(unDevice, &state, sizeof(state))) {
			bool* trackedDevices = InAM->GetTrackedDevices(); 
			trackedDevices[unDevice] = state.ulButtonPressed == 0;
		}
	}

	return bRet;
}

void Input::ProcessVREvent(const vr::VREvent_t & event, AssetManager* InAM) {
	switch (event.eventType) {
	case vr::VREvent_TrackedDeviceActivated:
	{
		InAM->SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
		printf("Device %u attached. Setting up render model.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		printf("Device %u detached.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceUpdated:
	{
		printf("Device %u updated.\n", event.trackedDeviceIndex);
	}
	break;
	}
}
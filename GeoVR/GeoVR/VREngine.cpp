#include "VREngine.h"



VREngine::VREngine() {
	WindowPos_X = 700;
	WindowPos_Y = 100;
	WindowSize_X = 640;
	WindowSize_Y = 320;
	VSync = false;
}


VREngine::~VREngine() {
}

bool VREngine::Init() {
	/* Init SDL */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	/* Init SteamVR */
	vr::EVRInitError eError = vr::VRInitError_None;
	HMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None) {
		HMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL);
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

	Window = SDL_CreateWindow("GeoVR", WindowPos_X, WindowPos_Y, WindowSize_X, WindowSize_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (Window == NULL) {
		printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	Context = SDL_GL_CreateContext(Window);
	if (Context == NULL) {
		printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	/* Init GLEW */
	glewExperimental = GL_TRUE;
	GLenum nGlewError = glewInit();
	if (nGlewError != GLEW_OK) {
		printf("%s - Error initializing GLEW! %s\n", __FUNCTION__, glewGetErrorString(nGlewError));
		return false;
	}
	glGetError(); // to clear the error caused deep in GLEW

	/* VSync */
	if (SDL_GL_SetSwapInterval(VSync ? 1 : 0) < 0) {
		printf("%s - Warning: Unable to set VSync! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	std::string Driver = "No Driver";
	std::string Display = "No Display";

	Driver = GetTrackedDeviceString(HMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	Display = GetTrackedDeviceString(HMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	SDL_SetWindowTitle(Window, ("hellovr - " + Driver + " " + Display).c_str());

	SceneInput = new Input(HMD);
	SceneWorld = new World(VSync);
	if (!SceneWorld->Init(HMD, Window, WindowSize_X, WindowSize_Y)) {
		printf("%s - Unable to initialize OpenGL!\n", __FUNCTION__);
		return false;
	}

	if (!SceneWorld->InitCompositor()) {
		printf("%s - Failed to initialize VR Compositor!\n", __FUNCTION__);
		return false;
	}
	return true;
}

void VREngine::Loop() {
	bool bQuit = false;

	SDL_StartTextInput();
	SDL_ShowCursor(SDL_DISABLE);

	while (!bQuit) {
		bQuit = SceneInput->HandleInput(SceneWorld->GetAssetManager());

		SceneWorld->RenderAll();
	}

	SDL_StopTextInput();
}

void VREngine::Shutdown() {
	if (HMD) {
		vr::VR_Shutdown();
		HMD = NULL;
	}

	SceneInput->~Input();
	SceneWorld->~World();

	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

std::string VREngine::GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError) {
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

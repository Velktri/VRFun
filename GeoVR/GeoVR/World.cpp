#include "World.h"

World::World(bool InVsync) {
	GRIDRADIUS_X = 10;
	GRIDRADIUS_Y = 10;
	GRIDSPACING = 1.0;
	Vsync = InVsync;
	GlFinishHack = true;
}

World::~World() {
	Scene->~Grid();
	ShaderManagement->~ShaderManager();
	TextureManagement->~TextureManager();
	AssetManagement->~AssetManager();
	WorldCamera->~Camera();
}

bool World::Init(vr::IVRSystem* InHMD, SDL_Window* InWindow, GLuint InWindowSize_X, GLuint InWindowSize_Y) {
	HMD = InHMD;
	WindowSize_X = InWindowSize_X;
	WindowSize_Y = InWindowSize_Y;
	Window = InWindow;

	WorldCamera = new Camera(HMD);
	Scene = new Grid(GRIDRADIUS_X, GRIDRADIUS_Y, GRIDSPACING);

	ShaderManagement = new ShaderManager();
	TextureManagement = new TextureManager();
	AssetManagement = new AssetManager(ShaderManagement->GetDefaultShader(), HMD);
	//SetupCompanionWindow();

	return true;
}

bool World::InitCompositor() {
	vr::EVRInitError peError = vr::VRInitError_None;

	if (!vr::VRCompositor()) {
		printf("Compositor initialization failed. See log file for details\n");
		return false;
	}

	return true;
}

AssetManager* World::GetAssetManager() {
	return AssetManagement;
}

void World::RenderAll() {
	// for now as fast as possible
	if (HMD) {
		//RenderControllerAxes();
		RenderHMDEyes();
		//RenderCompanionWindow();

		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t) WorldCamera->GetLeftFrame().m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)WorldCamera->GetRightFrame().m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	}

	if (Vsync && GlFinishHack) {
		//$ HACKHACK. From gpuview profiling, it looks like there is a bug where two renders and a present
		// happen right before and after the vsync causing all kinds of jittering issues. This glFinish()
		// appears to clear that up. Temporary fix while I try to get nvidia to investigate this problem.
		// 1/29/2014 mikesart
		glFinish();
	}

	// SwapWindow
	{
		SDL_GL_SwapWindow(Window);
	}

	// Clear
	{
		// We want to make sure the glFinish waits for the entire present to complete, not just the submission
		// of the command. So, we do a clear here right here so the glFinish will wait fully for the swap.
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Flush and wait for swap.
	if (Vsync) {
		glFlush();
		glFinish();
	}

	// Spew out the controller and pose count whenever they change.
	//if (m_iTrackedControllerCount != m_iTrackedControllerCount_Last || m_iValidPoseCount != m_iValidPoseCount_Last) {
	//	m_iValidPoseCount_Last = m_iValidPoseCount;
	//	m_iTrackedControllerCount_Last = m_iTrackedControllerCount;

	//	dprintf("PoseCount:%d(%s) Controllers:%d\n", m_iValidPoseCount, m_strPoseClasses.c_str(), m_iTrackedControllerCount);
	//}

	WorldCamera->UpdateHMDMatrixPose();

}

void World::RenderHMDEyes() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_MULTISAMPLE);

	// Left Eye
	glBindFramebuffer(GL_FRAMEBUFFER, WorldCamera->GetLeftFrame().m_nRenderFramebufferId);
	glViewport(0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight());
	RenderWorld(vr::Eye_Left);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, WorldCamera->GetLeftFrame().m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, WorldCamera->GetLeftFrame().m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight(), 0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight(),
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glEnable(GL_MULTISAMPLE);

	// Right Eye
	glBindFramebuffer(GL_FRAMEBUFFER, WorldCamera->GetRightFrame().m_nRenderFramebufferId);
	glViewport(0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight());
	RenderWorld(vr::Eye_Right);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, WorldCamera->GetRightFrame().m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, WorldCamera->GetRightFrame().m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight(), 0, 0, WorldCamera->GetFrameWidth(), WorldCamera->GetFrameHeight(),
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void World::RenderWorld(vr::Hmd_Eye Eye) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	/* Draw Scene */
	Scene->Draw(ShaderManagement->GetSceneShader(), WorldCamera, Eye);

	///* Draw Assets */
	//ShaderManagement->SetCurrentShader(NULL);
	//for each (Shader* s in ShaderManagement->GetUserShaderList()) {
	//	ShaderManagement->ShadeAssets(WorldCamera, s);
	//	AssetManagement->DrawAssets(s);
	//}

	///* Draw Lights */
	//LightManagement->ShadeLights(WorldCamera, ShaderManagement->GetLightShader());
	//LightManagement->Draw(ShaderManagement->GetLightShader());
}

void World::SetupCompanionWindow() {
	if (!HMD)
		return;

	std::vector<GLfloat> Verts;

	// left eye verts
	Verts.push_back(-1.0f);		Verts.push_back(-1.0f);		Verts.push_back(0.0f);		Verts.push_back(1.0f);
	Verts.push_back(0.0f);		Verts.push_back(-1.0f);		Verts.push_back(1.0f);		Verts.push_back(1.0f);
	Verts.push_back(-1.0f);		Verts.push_back(1.0f);		Verts.push_back(0.0f);		Verts.push_back(0.0f);
	Verts.push_back(0.0f);		Verts.push_back(1.0f);		Verts.push_back(1.0f);		Verts.push_back(0.0f);

	// right eye verts
	Verts.push_back(0.0f);		Verts.push_back(-1.0f);		Verts.push_back(0.0f);		Verts.push_back(1.0f);
	Verts.push_back(1.0f);		Verts.push_back(-1.0f);		Verts.push_back(1.0f);		Verts.push_back(1.0f);
	Verts.push_back(0.0f);		Verts.push_back(1.0f);		Verts.push_back(0.0f);		Verts.push_back(0.0f);
	Verts.push_back(1.0f);		Verts.push_back(1.0f);		Verts.push_back(1.0f);		Verts.push_back(0.0f);

	GLushort vIndices[] = { 0, 1, 3,   0, 3, 2,   4, 5, 7,   4, 7, 6 };
	CompanionWindowIndexSize = _countof(vIndices);

	glGenVertexArrays(1, &CompanionWindowVAO);
	glBindVertexArray(CompanionWindowVAO);

	glGenBuffers(1, &CompanionWindowVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CompanionWindowVBO);
	glBufferData(GL_ARRAY_BUFFER, Verts.size() * sizeof(GLfloat), &Verts[0], GL_STATIC_DRAW);

	glGenBuffers(1, &CompanionWindowEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CompanionWindowEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, CompanionWindowIndexSize * sizeof(GLushort), &vIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void *) 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void *) 2);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void World::RenderCompanionWindow() {
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, WindowSize_X, WindowSize_Y);

	glBindVertexArray(CompanionWindowVAO);
	glUseProgram(ShaderManagement->GetWindowShader()->GetShader());

	// render left eye (first half of index array )
	glBindTexture(GL_TEXTURE_2D, WorldCamera->GetLeftFrame().m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDrawElements(GL_TRIANGLES, CompanionWindowIndexSize / 2, GL_UNSIGNED_SHORT, 0);

	// render right eye (second half of index array )
	glBindTexture(GL_TEXTURE_2D, WorldCamera->GetRightFrame().m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDrawElements(GL_TRIANGLES, CompanionWindowIndexSize / 2, GL_UNSIGNED_SHORT, (const void *)(uintptr_t)(CompanionWindowIndexSize));

	glBindVertexArray(0);
	glUseProgram(0);
}

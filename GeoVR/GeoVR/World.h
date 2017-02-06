#pragma once
#include "Managers\ShaderManager.h"
#include "Managers\TextureManager.h"
#include "Managers\AssetManager.h"
#include "Models\Grid.h"
#include "Camera.h"

struct VertexDataSet {
	Vector2 position;
	Vector2 texCoord;

	VertexDataSet(const Vector2 & pos, const Vector2 tex) : position(pos), texCoord(tex) {}
};

class World {
public:
	World(bool InVsync);
	~World();

	bool Init(vr::IVRSystem* InHMD, SDL_Window* InWindow, GLuint InWindowSize_X, GLuint InWindowSize_Y);

	bool InitCompositor();
	AssetManager* GetAssetManager();
	void RenderAll();
	void RenderHMDEyes();
	void RenderWorld(vr::Hmd_Eye Eye);

private:
	vr::IVRSystem* HMD;
	Camera* WorldCamera;
	Grid* Scene;
	ShaderManager* ShaderManagement;
	TextureManager* TextureManagement;
	AssetManager* AssetManagement;
	bool Vsync;
	bool GlFinishHack;

	int GRIDRADIUS_X;
	int GRIDRADIUS_Y;
	float GRIDSPACING;
	void SetupCompanionWindow();
	void RenderCompanionWindow();

	GLuint CompanionWindowIndexSize;
	GLuint CompanionWindowVAO;
	GLuint CompanionWindowVBO;
	GLuint CompanionWindowEBO;
	GLuint WindowSize_X;
	GLuint WindowSize_Y;
	SDL_Window* Window;
};


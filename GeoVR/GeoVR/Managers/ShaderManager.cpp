#include "ShaderManager.h"



ShaderManager::ShaderManager() {
	BuildShaders();
}


ShaderManager::~ShaderManager() {
	for each (Shader* s in UserShaderList) {
		s->~Shader();
	}

	for each (Shader* s in SystemShaderList) {
		s->~Shader();
	}
}

Shader* ShaderManager::GetSceneShader() {
	return SceneShader;
}

Shader* ShaderManager::GetWindowShader() {
	return WindowShader;
}

std::vector<Shader*> ShaderManager::GetUserShaderList() {
	return UserShaderList;
}

Shader* ShaderManager::GetDefaultShader() {
	return DefaultShader;
}

void ShaderManager::BuildShaders() {
	SceneShader = new Shader("Assets/Shaders/Scene.vert", "Assets/Shaders/Scene.frag");
	//AssetShader = new Shader("assets/Shaders/Lighting.vert", "assets/Shaders/Lighting.frag");
	//LightShader = new Shader("assets/Shaders/Lamp.vert", "assets/Shaders/Lamp.frag");
	//ScreenShader = new Shader("assets/Shaders/Screen.vert", "assets/Shaders/Screen.frag");
	WindowShader = new Shader("Assets/Shaders/Window.vert", "Assets/Shaders/Window.frag");
	DefaultShader = new Shader("Assets/Shaders/Default.vert", "Assets/Shaders/Default.frag");

	SystemShaderList.push_back(SceneShader);
	SystemShaderList.push_back(WindowShader);
	//ShaderList.push_back(AssetShader);
	//SystemShaderList.push_back(LightShader);
	//SystemShaderList.push_back(ScreenShader);
	UserShaderList.push_back(DefaultShader);
}

void ShaderManager::ShadeAssets(Camera* WorldCamera, Shader* InCurrentShader) {
	if (InCurrentShader != CurrentShader) {
		CurrentShader = InCurrentShader;
		CurrentShader->Use();

		//glUniformMatrix4fv(CurrentShader->ShaderList["view"], 1, GL_FALSE, glm::value_ptr(WorldCamera->GetHMDMatrixProjectionEye(nEye)));
		//glUniformMatrix4fv(CurrentShader->ShaderList["projection"], 1, GL_FALSE, glm::value_ptr(WorldCamera->GetProjection()));
		//glm::vec3 pos = WorldCamera->GetPosition();
		//glUniform3f(CurrentShader->ShaderList["cameraPos"], pos.x, pos.y + 1, pos.z);
	}
}

Shader* ShaderManager::GetCurrentShader() {
	return CurrentShader;
}

void ShaderManager::SetCurrentShader(Shader * s) {
	CurrentShader = s;
}

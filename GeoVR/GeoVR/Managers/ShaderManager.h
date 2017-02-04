#pragma once
#include "../Models/Shader.h"
#include"../Camera.h"

class ShaderManager {
public:
	ShaderManager();
	~ShaderManager();

	Shader* GetDefaultShader();
	Shader* GetSceneShader();
	Shader* GetWindowShader();
	std::vector<Shader*> GetUserShaderList();
	void ShadeAssets(Camera* WorldCamera, Shader* InCurrentShader);
	Shader* GetCurrentShader();
	void SetCurrentShader(Shader* s);

private:
	Shader* DefaultShader;
	Shader* SceneShader;
	Shader* CurrentShader;
	Shader* WindowShader;

	std::vector<Shader*> UserShaderList;
	std::vector<Shader*> SystemShaderList;
	void BuildShaders();
};


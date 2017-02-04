#pragma once
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>

struct ShaderUniforms {
	std::string Type;
	std::string Name;
};

class Shader {
public:
	Shader(const char* vertSource, const char* fragSource);
	~Shader();

	void Use();
	GLuint GetShader();

	std::unordered_map<std::string, GLuint> ShaderList;

private:
	std::string ReadShaderFile(const char* filePath);
	void CheckUniform(std::string line);
	std::unordered_map<std::string, std::vector<std::string>> ParseStructs(std::string Shader);
	GLuint ShaderProgram;
	std::vector<ShaderUniforms> UniformList;
};


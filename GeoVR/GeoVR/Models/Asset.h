#pragma once
#include <GL\glew.h>
#include <string>
#include <openvr.h>
class Asset {
public:
	Asset(std::string InName);
	~Asset();

	std::string GetName();
	bool Init(const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture);


private:
	std::string Name;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint Texture;
	GLuint VertCount;
};


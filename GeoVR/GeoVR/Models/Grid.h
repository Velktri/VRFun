#pragma once
#include "../Models/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Camera.h"

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

class Grid {
public:
	Grid(int RowRadius, int ColumnRadius, float spacing);
	~Grid();

	void Draw(Shader* shader, Camera* worldCamera, vr::Hmd_Eye Eye);

private:
	GLuint VAO;
	GLuint VBO;
	std::vector<GLfloat> vertices;
	int vertSize;
	glm::mat4 orientation;
};


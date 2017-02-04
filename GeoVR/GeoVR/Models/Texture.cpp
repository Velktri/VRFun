#include "Texture.h"

Texture::Texture(std::string texturePath) {
	unsigned nError = lodepng::decode(imageRGBA, textureWidth, textureHeight, texturePath.c_str());

	if (nError != 0) {
		std::cout << "Texture Loading Failed: Could not find " << texturePath << std::endl;
	} else {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageRGBA[0]);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


Texture::~Texture() {
	glDeleteTextures(1, &texture);
}

GLuint Texture::GetTexture() {
	return texture;
}

std::string Texture::GetType() {
	return type;
}

void Texture::SetType(std::string InType) {
	type = InType;
}

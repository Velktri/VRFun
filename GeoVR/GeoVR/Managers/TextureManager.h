#pragma once
#include <vector>
#include <iostream>
#include "../Models/Texture.h"
class TextureManager {
public:
	TextureManager();
	~TextureManager();

private:
	void BuildTexture(std::string path);
	std::vector<Texture*> TextureList;
};


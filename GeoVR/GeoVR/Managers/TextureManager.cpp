#include "TextureManager.h"



TextureManager::TextureManager() {
}


TextureManager::~TextureManager() {
	for each (Texture* t in TextureList) {
		t->~Texture();
	}
}

void TextureManager::BuildTexture(std::string path) {
	TextureList.push_back(new Texture(path));
}
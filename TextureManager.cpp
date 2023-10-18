#include "TextureManager.h"
#include <cassert>
#include<fstream>
#include<sstream>


TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize() {
}

void TextureManager::Finalize() {

}


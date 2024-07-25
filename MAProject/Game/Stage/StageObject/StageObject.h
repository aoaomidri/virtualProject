#pragma once
#include"LevelLoader/LevelLoader.h"
#include"Object3D.h"

class StageObject{
public:
	//初期化
	void Initialize();

	void Draw(const ViewProjection& viewProjection);

};


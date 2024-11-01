#pragma once
#include"LevelLoader/LevelLoader.h"
#include"Object3D.h"
/*ステージに配置するオブジェクトの描画*/

class StageObject{
public:
	//初期化
	void Initialize();

	void Draw(const ViewProjection& viewProjection);

};


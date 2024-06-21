#pragma once
#include"BaseScene/BaseScene.h"
class TitleScene : public BaseScene {
	//初期化
	void Initialize()override;

	//終了
	void Finalize()override;

	//毎フレーム更新
	void Update()override;

	//描画
	void Draw()override;

	//ゲーム全体にかかわるImgui
	void Debug() override;
};


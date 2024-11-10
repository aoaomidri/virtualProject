#pragma once
#include"BaseScene/BaseScene.h"
#include"DirectXCommon.h"
#include"Input.h"
#include"TextureManager.h"
#include"Sprite.h"
#include"Object3D.h"
#include"Matrix.h"
#include"FollowCamera.h"
#include"FloorManager.h"
#include"LockOn.h"
#include"ParticleBase.h"
#include"SkyBox.h"
#include"Audio.h"
#include"LevelLoader/LevelLoader.h"
#include"StageObject.h"
#include"SceneManager/SceneManager.h"
#include"PostEffect.h"

/*タイトル部分の初期化、更新、描画*/


class TitleScene : public BaseScene {
	void TextureLoad();
	void SoundLoad();
	void SpriteInitialize();
	void ObjectInitialize();
	//初期化
	void Initialize()override;

	//終了
	void Finalize()override;

	//毎フレーム更新
	void Update()override;

	//3D描画
	void AllDraw3D()override;

	//2D描画
	void AllDraw2D()override;

	//ゲーム全体にかかわるImgui
	void Debug() override;
	void DrawParticle();
	void DrawSkin3D();
	void Draw3D();
	void Draw2D();


private:
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//テクスチャマネージャー
	PostEffect* postEffect_ = nullptr;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<FloorManager> floorManager_;

	EulerTransform firstFloor_ = {
		.scale = {50.0f,1.0f,50.0f},
		.rotate = {0},
		.translate = {0,0.f,0.0f}
	};

	bool isFloorMove_ = false;

	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	uint32_t titleBGM_{};

	std::unique_ptr<SkyBox> skyBox_;

	//スプライト関連
	//タイトル
	std::unique_ptr<Sprite> titleSprite_;

	std::unique_ptr<Sprite> startSprite_;

	std::unique_ptr<Sprite> selectSprite_;

	bool isReset_ = false;

	bool isStart_ = true;

	std::unique_ptr<Sprite> comboSprite_;

	std::unique_ptr<Sprite> controlSprite_;

	std::unique_ptr<Sprite> backSprite_;

	std::unique_ptr<Sprite> fadeSprite_;
	float fadeAlpha_ = 0.0f;
	bool isFade_ = false;



	/*ロックオン*/
	std::unique_ptr<LockOn> lockOn_;

	std::unique_ptr<StageObject> stageObject_;

};


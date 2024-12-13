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
/*リザルト部分の初期化、更新、描画*/

class ResultScene : public BaseScene{
	//テクスチャのロード
	void TextureLoad();
	//サウンドのロード
	void SoundLoad();
	//画像の初期化
	void SpriteInitialize();
	//3Dオブジェの初期化
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
	//imguiの描画
	void DrawImgui();
	//パーティクルの描画
	void DrawParticle();
	//スキニング処理されたオブジェの描画
	void DrawSkin3D();
	//オブジェの描画
	void Draw3D();
	//テクスチャの描画
	void Draw2D();

private:
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;
	//カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//床
	std::unique_ptr<FloorManager> floorManager_;

	EulerTransform firstFloor_ = {
		.scale = {50.0f,1.0f,50.0f},
		.rotate = {0},
		.translate = {0,0.f,0.0f}
	};
	//床を動かすか
	bool isFloorMove_ = false;
	//入力
	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	//スプライト関連
	//クリア
	std::unique_ptr<Sprite> clearSprite_;
	//プレス
	std::unique_ptr<Sprite> pressSprite_;
	std::array<std::unique_ptr<Sprite>, 4> timerTexs_;

	/*ロックオン*/
	std::unique_ptr<LockOn> lockOn_;
	//ステージ
	std::unique_ptr<StageObject> stageObject_;
};


#pragma once
#include"DirectXCommon.h"
#include"Input.h"
#include"TextureManager.h"
#include"Sprite.h"
#include"Object3D.h"
#include"Matrix.h"
#include"OBB.h"
#include"Quaternion.h"
#include"FollowCamera.h"
#include"Player.h"
#include"Enemy.h"
#include"FloorManager.h"
#include"LockOn.h"
#include"ParticleBase.h"
#include"Audio.h"

class GameScene{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon_);
	//更新
	void Update();

	void AudioDataUnLoad();

	//パーティクルの描画
	void DrawParticle();

	//3Dオブジェクトの描画
	void Draw3D();
	//テクスチャ関連の描画
	void Draw2D();
	//削除をまとめた関数
	void Finalize();

private:
	//クラス内関数
	//テクスチャ読み込み
	void TextureLoad();

	//音読み込み
	void SoundLoad();

	//スプライト初期化
	void SpriteInitialize(DirectXCommon* dxCommon_);

	//オブジェクトの初期化
	void ObjectInitialize(DirectXCommon* dxCommon_);

	//imguiの描画
	void DrawImgui();

private:

private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	// 
	//自機のモデル
	std::unique_ptr<Object3D> TestObj_;
	std::unique_ptr<Object3D> TestGroundObj_;

	Model* TestModel_;
	Model* TestGroundModel_;

	Transform testTransform_;
	Transform testGroundTransform_;

	Matrix4x4 testMatrix_;
	Matrix4x4 testGroundMatrix_;

	//テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager_;

	std::unique_ptr<FollowCamera> followCamera_;

	DirectionalLight directionalLight_;

	PointLight pointLight_;

	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	uint32_t titleBGM{};

	//スプライト関連
	//タイトル
	std::unique_ptr<Sprite> titleSprite_;

	std::unique_ptr<Sprite> fadeSprite_;
	float fadeAlpha_ = 0.0f;
	bool isFade_ = false;

	std::unique_ptr<Sprite> pressSprite_;

	std::unique_ptr<Sprite> clearSprite_;

	//シーン関連
	enum SceneName{
		TITLE,
		GAME,
		CLEAR
	};

	int sceneNum_;
};


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
	void Initialize();
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
	void SpriteInitialize();

	//オブジェクトの初期化
	void ObjectInitialize();

	//imguiの描画
	void DrawImgui();


	/*分ける予定があるけどひとまずのやつ*/
	
	//OBB同士の当たり判定
	

private:
	/*ファイル関連*/
	int stageSelect_;

	std::string stageName_;

	std::vector<std::string> stages_;

private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	// 
	//自機のモデル
	std::unique_ptr<Object3D> TestObj_;
	std::unique_ptr<Object3D> TestGroundObj_;
	std::unique_ptr<Object3D> pointLightObj_;

	std::unique_ptr<Model> TestModel_;
	std::unique_ptr<Model> TestGroundModel_;

	Transform testTransform_;
	Transform testGroundTransform_;
	Transform pointLightTransform_;

	Matrix4x4 testMatrix_;
	Matrix4x4 testGroundMatrix_;
	Matrix4x4 pointLightMatrix_;

	//テクスチャマネージャー
	

	std::unique_ptr<FollowCamera> followCamera_;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<ParticleBase> particle_;

	std::unique_ptr<FloorManager> floorManager_;

	Transform firstFloor_ = {
		.scale = {2.0f,0.5f,2.0f},
		.rotate = {0},
		.translate = {0}
	};

	bool isFloorMove_ = false;

	int chackCollision = 0;

	DirectionalLight directionalLight_;

	PointLight pointLight_;
	Transform particleTrnadform_ = {
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	uint32_t titleBGM{};

	uint32_t gameBGM{};

	//天球
	std::unique_ptr<Object3D> skyDomeObj_;

	std::unique_ptr<Model> skyDomeModel_;

	Transform skyDomeTrnasform_;

	Matrix4x4 skyDomeMatrix_;

	//スプライト関連
	//タイトル
	int changeNumber_ = 0;

	std::unique_ptr<Sprite> titleSprite_;

	std::unique_ptr<Sprite> fadeSprite_;
	float fadeAlpha_ = 0.0f;
	bool isFade_ = false;

	std::unique_ptr<Sprite> pressSprite_;

	std::unique_ptr<Sprite> clearSprite_;

	std::unique_ptr<Sprite> actionTextSprite_;

	std::unique_ptr<Sprite> attackSprite_;

	//シーン関連
	enum SceneName{
		TITLE,
		GAME,
		CLEAR
	};

	int sceneNum_;
};


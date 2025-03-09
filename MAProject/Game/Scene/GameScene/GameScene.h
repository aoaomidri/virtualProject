#pragma once
#include"BaseScene/BaseScene.h"
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
#include"EnemyManager.h"
#include"FloorManager.h"
#include"LockOn.h"
#include"ParticleBase.h"
#include"SkyBox.h"
#include"Audio.h"
#include"LevelLoader/LevelLoader.h"
#include"SceneManager/SceneManager.h"
#include"StageObject.h"
#include"PostEffect.h"
#include"UI/GameUIManager.h"
/*インゲーム部分の初期化、更新、描画*/

class GameScene : public BaseScene{
public:
	//初期化
	void Initialize()override;
	//更新
	void Update()override;

	//削除をまとめた関数
	void Finalize()override ;

	//3D描画
	void AllDraw3D()override;

	//2D描画
	void AllDraw2D()override;

	//ゲーム全体にかかわるImgui
	void Debug() override;
	//パーティクルの描画
	void DrawParticle();

	void DrawSkin3D();

	//3Dオブジェクトの描画
	void Draw3D();


	//テクスチャ関連の描画
	void Draw2D();

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
	//時間画像の更新
	void TimeTexUpdate();

	/*分ける予定があるけどひとまずのやつ*/
	void AllCollision();
	void FilesSave(const std::vector<std::string>& stages);
	void FilesOverWrite(const std::string& stage);
	void FilesLoad(const std::string& stage);


private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Sprite> backSprite_;
	std::array<std::unique_ptr<Sprite>, 4> timerTexs_;
	std::unique_ptr<Sprite> slushTex_;

	std::unique_ptr<GameUIManager> uiManager_;

	/*ロックオン*/
	std::unique_ptr<LockOn> lockOn_;

	std::unique_ptr<StageObject> stageObject_;

	std::unique_ptr<FloorManager> floorManager_;

	//プレイヤー
	std::unique_ptr<Player> player_;
	//敵
	std::unique_ptr<EnemyManager> enemyManager_;

	Vector2 playerAttackShake_ = { 1.0f,3.0f };

	Vector2 enemyAttackShake_ = { 0.5f,1.5f };
	

	EulerTransform firstFloor_ = {
		.scale = {1.0f,0.5f,1.0f},
		.rotate = {0},
		.translate = {0.0,0.f,0.0f}
	};

	int chackCollision = 0;

	EulerTransform particleTrnadform_ = {
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;
	//ゲームのBGM
	uint32_t gameBGM_{};
	//プレイヤーの攻撃が当たったときのSE
	uint32_t enemyHitSE_{};
	float seVolume_ = 0.3f;
	
	bool isFloorMove_ = false;
	//スプライト関連
	bool isReset_ = false;
	bool isStart_ = true;
	

};


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
#include"Enemy.h"
#include"FloorManager.h"
#include"LockOn.h"
#include"ParticleBase.h"
#include"SkyBox.h"
#include"Audio.h"
#include"LevelLoader/LevelLoader.h"
#include"StageObject.h"
#include"PostEffect.h"
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


	/*分ける予定があるけどひとまずのやつ*/
	void AllCollision();
	void FilesSave(const std::vector<std::string>& stages);
	void FilesOverWrite(const std::string& stage);
	void FilesLoad(const std::string& stage);
	//OBB同士の当たり判定
	bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2);

private:
	/*ファイル関連*/
	int stageSelect_;

	std::string stageName_;

	std::vector<std::string> stages_;

private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//テクスチャマネージャー
	PostEffect* postEffect_ = nullptr;

	std::unique_ptr<FollowCamera> followCamera_;	
	
	//std::unique_ptr<ParticleBase> particle_;

	std::unique_ptr<FloorManager> floorManager_;

	EulerTransform firstFloor_ = {
		.scale = {50.0f,1.0f,50.0f},
		.rotate = {0},
		.translate = {0,0.f,0.0f}
	};

	bool isFloorMove_ = false;

	std::unique_ptr<Player> player_;

	std::unique_ptr<Enemy> enemy_;
	std::list<std::unique_ptr<Enemy>> enemies_;

	static const size_t enemyNum_ = 20;

	std::array<Vector3, enemyNum_> enemysPos_;

	int chackCollision = 0;


	EulerTransform particleTrnadform_ = {
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	uint32_t titleBGM{};

	uint32_t gameBGM{};

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

	std::unique_ptr<Sprite> pressSprite_;

	std::unique_ptr<Sprite> clearSprite_;

	std::unique_ptr<Sprite> actionTextSprite_;

	std::unique_ptr<Sprite> attackSprite_;

	/*ロックオン*/
	std::unique_ptr<LockOn> lockOn_;

	std::unique_ptr<StageObject> stageObject_;

	//シーン関連
	enum SceneName{
		TITLE,
		GAME,
		CLEAR
	};

	int sceneNum_;



};


#pragma once
#include"DirectXCommon.h"
#include"Input.h"
#include"TextureManager.h"
#include"Sprite.h"
#include"Object3D.h"
#include"math/Matrix.h"
#include"OBB.h"
#include"math/Quaternion.h"
#include"FollowCamera.h"
#include"Player.h"

class GameScene{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon_);
	//更新
	void Update(Input* input_);
	//3Dオブジェクトの描画
	void Draw3D();
	//テクスチャ関連の描画
	void Draw2D();

private:
	//クラス内関数
	//テクスチャ読み込み
	void TextureLoad();
	//imguiの描画
	void DrawImgui();
	//OBB同士の当たり判定
	bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2);

private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	//テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager_;
	

	//3Dオブジェクトの宣言
	/*それぞれに持たせる予定だが突貫工事なのでここに書いている*/
	//ステージの床
	std::unique_ptr<Object3D> floor_[3];
	//ゴール
	std::unique_ptr<Object3D> goal_;
	//敵
	std::unique_ptr<Object3D> enemy_;
	//敵の部品
	std::unique_ptr<Object3D> enemyParts_;
	//天球
	std::unique_ptr<Object3D> skyDome_;

	//テクスチャの宣言
	std::unique_ptr<Sprite> testTexture_;

	//プレイヤー
	std::unique_ptr<Player> player_;

	//プレイヤーを追うカメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//数値系
	/*分離予定なので適当に置いている*/
	Vector2 spritePosition_ = { 100.0f,100.0f };
	float spriteRotate_ = 0.0f;
	Vector2 spriteScale_ = { 640.0f,360.0f };
	Vector2 spriteAnchorPoint_ = { 0.5f,0.5f };
	Vector4 spriteColor_ = { 1.0f,1.0f,1.0f,1.0f };
	bool isSpriteDraw = true;

	Transform floorTransform[3] = {};
	
	Transform EnemyTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,3.14f,0.0f},
		.translate = {0.0f,0.8f,7.0f}
	};

	Transform EnemyPartsTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,1.57f},
		.translate = {0.0f,1.7f,7.0f}
	};

	Transform goalTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.65f,9.0f}
	};

	Transform skyDomeTransform{
		.scale = {20.0f,20.0f,20.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};



	Transform cameraTransform{
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	OBB enemyOBB = {};

	OBB floorOBB[3] = {};

	OBB goalOBB = {};

	Matrix4x4 enemyMatrix{};

	Matrix4x4 enemyPartsMatrix{};

	Matrix4x4 moveFloorMatrix[3]{};

	Matrix4x4 movePlayerMatrix{};

	Matrix4x4 moveFloorTransformMatrix{};

	Matrix4x4 skyDomeMatrix{};

	Matrix4x4 goalMatrix{};

	Vector3 FloorPlayerPosition{};

	float Magnification = 1.0f;

	float MagnificationY = 1.0f;

	Vector3 moveSpeed_ = { 0.02f,0.05f,0.0f };

	float EnemyMagnification = 1.0f;

	float EnemyMoveSpeed_ = 0.02f;

	Vector3 cameraMove_{};

	const Vector3 baseCameraOffset = { 0.0f,1.0f,-8.0f };

	Vector3 cameraOffset{};

	Vector3 vector_;

	int chackCollision = 0;

	bool isDown = true;

	bool isDead = false;

	Vector3 axis{};

	float angle = 0.45f;

	Vector3 pointY_{};
	Vector3 rotateByQuaternion_{};
	Vector3 rotateByMatrix_{};

	Matrix4x4 rotateMatrix_;

	Quaternion rotation0_;
	Quaternion rotation1_;

	Quaternion interpolate_[5];
	
};


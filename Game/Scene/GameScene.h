#pragma once
#include"../../Engine/Base/DirectXCommon.h"
#include"../../Game/Input/Input.h"
#include"../../Engine/2D/TextureManager.h"
#include"../../Engine/2D/Sprite.h"
#include"../../Engine/3D/Object3D/Object3D.h"
#include"../../math/Matrix.h"
#include"../../Engine/3D/Shape/OBB.h"
#include"../../math/Quaternion.h"
#include"../Camera/FollowCamera.h"
#include"../Character/Player.h"
#include"../Character/Enemy.h"
#include"../Stage/Floor/FloorManager.h"
#include"../Camera/LockOn.h"
#include"../../Engine/3D/Object3D/ParticleBase.h"
#include"../Audio/Audio.h"

class GameScene{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon_);
	//更新
	void Update(Input* input_);

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

	//オブジェクトの初期化
	void ObjectInitialize(DirectXCommon* dxCommon_);

	//imguiの描画
	void DrawImgui();


	/*分ける予定があるけどひとまずのやつ*/
	void AllCollision();
	//OBB同士の当たり判定
	bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2);

private:
	//クラス内変数
	//基本の宣言はユニークポインタで
	//テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager_;

	std::unique_ptr<FollowCamera> followCamera_;
	
	std::unique_ptr<ParticleBase> particle_;

	std::unique_ptr<Object3D> obj_;

	Model* model_ = nullptr;

	Model* boxModel_ = nullptr;

	Transform objectTrnadform_ = {
		.scale = {5.0f,5.0f,5.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Transform particleTrnadform_ = {
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Matrix4x4 objMatrix_{};


	//音関係
	Audio* audio_ = nullptr;

	SoundData soundData1{};
};


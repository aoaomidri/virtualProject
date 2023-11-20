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
#include"Character/Player/Player.h"
#include"Character/Player/InputHandle.h"

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

	std::unique_ptr<Player> player_;
	
	//数値系
	/*分離予定なので適当に置いている*/
	Vector2 spritePosition_ = { 100.0f,100.0f };
	float spriteRotate_ = 0.0f;
	Vector2 spriteScale_ = { 640.0f,360.0f };
	Vector2 spriteAnchorPoint_ = { 0.5f,0.5f };
	Vector4 spriteColor_ = { 1.0f,1.0f,1.0f,1.0f };
	bool isSpriteDraw = true;

};


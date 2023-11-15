#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"math/Matrix.h"
#include"Object3D.h"
#include"Input.h"
#include"OBB.h"
#include"Adjustment_Item.h"
#include"math/Quaternion.h"
#include<optional>

class Player {
public:
	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//更新処理
	void Update(Input* input);
	//描画
	void Draw(TextureManager* textureManager, const Transform& cameraTransform);
	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn();

public:
	//Getter
	const Transform& GetTransform() const { return playerTransform_; }

	const Vector3& GetTranslate() const { return playerTransform_.translate; }

	const Vector3& GetRotate() const { return playerTransform_.rotate; }

	const Vector3& GetScale() const { return playerTransform_.scale; }

	const OBB& GetOBB()const { return playerOBB_; }

	const OBB& GetWeaponOBB()const { return weaponOBB_; }

	//Setter

	void SetCameraTransform(const Transform* cameraTransform) { cameraTransform_ = cameraTransform; }

	void SetIsDown(bool isDown) { isDown_ = isDown; }

private:
	//クラス内関数

	//通常行動初期化
	void BehaviorRootInitialize();
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//ダッシュ行動初期化
	void BehaviorDashInitialize();
	// 通常行動更新
	void BehaviorRootUpdate(Input* input);
	//攻撃行動更新
	void BehaviorAttackUpdate();
	//ダッシュ行動更新
	void BehaviorDashUpdate();
private:
	//自機のモデル
	std::unique_ptr<Object3D> playerModel_;
	//武器のモデル
	std::unique_ptr<Object3D> weaponModel_;
	std::unique_ptr<Object3D> weaponCollisionModel_;


	//自機のSRT
	Transform playerTransform_{};
	//武器のSRT
	Transform weaponTransform_{};
	Transform weaponCollisionTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};
	Matrix4x4 testPlayerMatrix_{};
	Matrix4x4 testPlayerRotateMatrix_{};

	Matrix4x4 playerScaleMatrix_{};
	Matrix4x4 playerRotateMatrix_{};
	Matrix4x4 playerTransformMatrix_{};

	//武器のマトリックス
	Matrix4x4 weaponMatrix_{};
	Matrix4x4 weaponCollisionMatrix_{};

	//スケールを無視したマトリックス
	Matrix4x4 playerMoveMatrix_{};

	//自機の移動
	Vector3 move_{};
	Vector3 frontMove_{};

	const Transform* cameraTransform_ = nullptr;

	//自機のOBB
	OBB playerOBB_{};

	//武器のOBB
	OBB weaponOBB_{};

	//移動スピード
	const float moveSpeed_ = 0.1f;

	//ダッシュ時のスピード倍率
	float kDashSpeed = 15.0f;
	//ダッシュのクールタイム
	int kDashCoolTime = 20;

	int dashCoolTime;

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;

	/*武器に関連するもの*/
	//武器の回転
	float weapon_Rotate = 0.0f;
	float arm_Rotate = -3.15f;
	//武器開店に関連する変数
	Vector3 Weapon_offset;
	Vector3 Weapon_offset_Base = { 0.0f,4.0f, 0.0f };

	const float moveWeapon = 0.1f;
	const float moveWeaponShakeDown = 0.2f;
	const float MaxRotate = 1.55f;
	const float MinRotate = -0.6f;

	int WaitTimeBase = 20;
	int WaitTime = 0;

	bool isShakeDown = false;

	//落下するかどうか
	bool isDown_ = false;

	/*振る舞い系*/
	enum class Behavior {
		kRoot,		//通常状態
		kAttack,	//攻撃中
		kDash,		//ダッシュ中
	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

};


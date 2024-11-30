#pragma once
#include"FollowCamera.h"
#include"Sprite.h"
#include"OBB.h"
#include"Shape/Collider.h"
#include"Enemy.h"

/*ロックオンの更新*/
class LockOn{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, Input* input, const ViewingFrustum& viewingFrustum, const bool isAvoid, const uint32_t serialNumber);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void DrawImgui();


	
private:
	/// <summary>
	/// 検索
	/// </summary>
	void search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum);

	//ジャスト回避したときの検索
	void avoidSearch(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const uint32_t serialNumber);

	/// <summary>
	/// 範囲内判定
	/// </summary>
	bool InTarget(const OBB enemyOBB, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum);
	

public:
	/// <summary>
/// ターゲットリセット
/// </summary>
	void TargetReset();

	Vector3 GetTargetPosition() const;

	bool ExistTarget() const { return target_ ? true : false; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetViewingFrustum(const ViewingFrustum* viewingFrustum) { viewingFrustum_ = viewingFrustum; }

public:
	//ロック御大将
	const Enemy* target_ = nullptr;

private:
	//ロックオンマーク用スプライト

	std::unique_ptr<Object3D> LockOnObj_;

	EulerTransform lockOnTransfrom_;

	Matrix4x4 lockOnMatrix_;

	const ViewProjection* viewProjection_ = nullptr;

	const ViewingFrustum* viewingFrustum_ = nullptr;

	Vector2 screenPos_;

	std::list<std::pair<float, const Enemy*>> targets;

	std::list<std::pair<float, const Enemy*>>::iterator it;

	bool autoLockOn_ = false;

	float easeT_ = 0.0f;

	float easeY_ = 0.0f;

	Vector2 MinMax_ = { 0.0f,1.0f };

	bool isLockOn_ = false;

	float length = 0;
};


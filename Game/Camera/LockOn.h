#pragma once
#include"FollowCamera.h"
#include"Sprite.h"
#include"OBB.h"
#include"Enemy.h"
/// <summary>
/// ロックオン
/// </summary>
class LockOn{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, Input* input, const ViewingFrustum& viewingFrustum);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(TextureManager* textureManager);

	void DrawImgui();

	bool IsCollisionViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum);

	bool IsCollisionOBB(const OBB& obb, const ViewingFrustum& viewingFrustum);

	bool IsCollisionOBBViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum);
	
private:
	/// <summary>
	/// 検索
	/// </summary>
	void search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum);

	/// <summary>
	/// 範囲内判定
	/// </summary>
	bool InTarget(const OBB enemyOBB, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum);
	/// <summary>
	/// ターゲットリセット
	/// </summary>
	void TargetReset(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum);

public:
	Vector3 GetTargetPosition() const;

	bool ExistTarget() const { return target_ ? true : false; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetViewingFrustum(const ViewingFrustum* viewingFrustum) { viewingFrustum_ = viewingFrustum; }

public:
	//ロック御大将
	const Enemy* target_ = nullptr;

private:
	//ロックオンマーク用スプライト
	std::unique_ptr<Sprite> lockOnMark_;

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


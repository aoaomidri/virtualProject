#pragma once
#include"../../FollowCamera.h"
#include"../../Sprite.h"
#include"../Character/Enemy.h"
/// <summary>
/// ロックオン
/// </summary>
class LockOn{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(TextureManager* textureManager);
	
private:
	//ロックオンマーク用スプライト
	std::unique_ptr<Sprite> lockOnMark_;

	//ロック御大将
	const Enemy* target_ = nullptr;
};


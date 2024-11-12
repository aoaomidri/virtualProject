#pragma once
/*プレイヤーの状態の基本設定*/
//前方宣言
class PlayerStateManager;

class IPlayerState{
public:
	enum class StateName{
		Root,
		Attack,
		StrongAttack,
		Dash,
	};

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	virtual void SetStateManager(PlayerStateManager* stateManager) { stateManager_ = stateManager; }

public:
	virtual ~IPlayerState() = default;

private:
	//ステートマネージャー
	PlayerStateManager* stateManager_ = nullptr;
};


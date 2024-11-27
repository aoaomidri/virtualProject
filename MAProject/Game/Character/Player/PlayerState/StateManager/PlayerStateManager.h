#pragma once
#include"IPlayerState.h"
#include"PlayerRoot.h"
#include"PlayerDash.h"
#include"PlayerAttack.h"
#include"PlayerStrongAttack.h"
#include<memory>

class PlayerStateManager{
public:
	using StateName = IPlayerState::StateName;
	using PLContext = IPlayerState::PlayerContext;

	PlayerStateManager() = default;
	~PlayerStateManager() = default;
	PlayerStateManager(const PlayerStateManager& stateManager) = delete;
	PlayerStateManager& operator=(const PlayerStateManager&) = delete;

	static PlayerStateManager* GetInstance() {
		static PlayerStateManager instance;
		return &instance;
	}

	void ChangeState(const StateName nextState);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:
	//今の状態
	std::unique_ptr<IPlayerState> nowState_;
	//次の状態
	std::unique_ptr<IPlayerState> nextState_;

	PLContext context_;

};


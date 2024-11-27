#include "PlayerStateManager.h"
#include<assert.h>
void PlayerStateManager::ChangeState(const StateName nextState){
	assert(nextState_ == nullptr);
	if (nextState == StateName::Root){
		nextState_ = std::make_unique<PlayerRoot>(context_);
	}
	else if (nextState == StateName::Attack){
		nextState_ = std::make_unique<PlayerAttack>(context_);
	}
	else if (nextState == StateName::StrongAttack){
		nextState_ = std::make_unique<PlayerStrongAttack>(context_);
	}
	else if (nextState == StateName::Dash){
		nextState_ = std::make_unique<PlayerDash>(context_);
	}
}

void PlayerStateManager::Update(){
	/*状態切り替え処理*/
	if (nextState_) {
		//状態切り替え
		nowState_ = std::move(nextState_);
		nextState_ = nullptr;
		nowState_->SetStateManager(this);
		//次の状態を初期化する
		nowState_->Initialize();
	}

	//実行中の状態を更新する
	nowState_->Update();
}

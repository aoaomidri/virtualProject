#include "PlayerStateManager.h"

void PlayerStateManager::ChangeState(const StateName nextState){
	if (nextState == StateName::Root){
		nextState_ = std::make_unique<PlayerRoot>();
	}
	else if (nextState == StateName::Attack){

	}
	else if (nextState == StateName::StrongAttack){

	}
	else if (nextState == StateName::Dash){

	}
}

void PlayerStateManager::Update(){

}

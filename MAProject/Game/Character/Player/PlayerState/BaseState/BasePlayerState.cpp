#include "BasePlayerState.h"

void BasePlayerState::Initialize(){
	//もうすでに読み込んでいたら行わない
	if (!input_){
		input_ = Input::GetInstance();
	}
	if (!audio_){
		audio_ = Audio::GetInstance();
		avoidSE_ = audio_->LoadAudio("SE/avoidSE.mp3");
		attackMotionSE_ = audio_->LoadAudio("SE/attackMotionSE.mp3");
		playerHitSE_ = audio_->LoadAudio("SE/playerHitSE.mp3");
	}
	if (!context_.trail_) {
		context_.trail_ = std::make_unique<TrailEffect>();
	}
}



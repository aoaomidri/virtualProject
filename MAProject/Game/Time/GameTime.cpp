#include "GameTime.h"

// 初期化
float GameTime::deltaTime_ = 1.0f / 60.0f;  // 60 FPSで約16ms
float GameTime::timeScale_ = 1.0f;
float GameTime::duration_ = 0.0f;
float GameTime::elapsedTime_ = 0.0f;
bool GameTime::isStop_ = false;
bool GameTime::isSlowDown_ = false;

void GameTime::Update(){
	if (isStop_){
		/*停止の処理*/
		elapsedTime_ += deltaTime_;
		if (duration_ < elapsedTime_){
			isStop_ = false;
		}

		timeScale_ = 0.0f;
	}
	else if (isSlowDown_){
		/*スロー化の処理b*/
		elapsedTime_ += deltaTime_;
		if (duration_ < elapsedTime_) {
			isSlowDown_ = false;
		}
	}
	else {
		/*元の倍率*/		
		timeScale_ = 1.0f;
		
	}
}

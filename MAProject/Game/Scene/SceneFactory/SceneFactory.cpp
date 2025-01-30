#include "SceneFactory.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const SceneName sceneName){
	std::unique_ptr<BaseScene> newScene = nullptr;
	behaviorRequest_ = sceneName;
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case SceneName::Title:
			newScene = std::make_unique<TitleScene>();
			break;
		case SceneName::Game:
			newScene = std::make_unique<GameScene>();
			break;
		case SceneName::Result:
			newScene = std::make_unique<ResultScene>();
			break;		
		}
	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;
	return newScene;
}

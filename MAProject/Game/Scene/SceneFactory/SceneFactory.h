#pragma once
#include"AbstractSceneFactory.h"
#include<optional>
#include"TitleScene/TitleScene.h"
#include"GameScene/GameScene.h"
#include"ResultScene/ResultScene.h"

class SceneFactory : public AbstractSceneFactory{
public:
	//シーンの生成
	std::unique_ptr<BaseScene> CreateScene(const SceneName sceneName)override;

private:
	SceneName behavior_ = SceneName::Title;

	std::optional<SceneName> behaviorRequest_ = SceneName::Title;
};


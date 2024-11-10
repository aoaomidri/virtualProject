#pragma once
#include"AbstractSceneFactory.h"
#include<optional>
#include"TitleScene/TitleScene.h"
#include"GameScene/GameScene.h"

class SceneFactory : public AbstractSceneFactory{
public:

	std::unique_ptr<BaseScene> CreateScene(const SceneName sceneName)override;

private:
	SceneName behavior_ = SceneName::Title;

	std::optional<SceneName> behaviorRequest_ = SceneName::Title;
};


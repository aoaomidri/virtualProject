#pragma once
#include"BaseScene/BaseScene.h"
#include<string>
#include <memory>
class AbstractSceneFactory{
public:
	using SceneName = BaseScene::SceneName;
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;

	virtual std::unique_ptr<BaseScene> CreateScene(const SceneName sceneName) = 0;

};


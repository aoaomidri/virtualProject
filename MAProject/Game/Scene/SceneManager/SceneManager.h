#pragma once
#include"BaseScene/BaseScene.h"
#include"AbstractSceneFactory/AbstractSceneFactory.h"
#include <memory>
#include"Sprite.h"

/*シーンを管理するクラス*/
class SceneManager{
public:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager& TextureManager) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	static SceneManager* GetInstance() {
		static SceneManager instance;
		return &instance;
	}

	//次シーン予約
	void ChangeScene(const AbstractSceneFactory::SceneName nextScene);
	//シーン生成機構のセット
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	//更新処理
	void Update();
	//3D描画処理
	void Draw3D();
	//2D描画処理
	void Draw2D();
	//解放処理
	void Finalize();
private:
	//今のシーン(実行中シーン)
	std::unique_ptr<BaseScene> scane_;
	//次のシーン
	std::unique_ptr<BaseScene> nextScane_;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};


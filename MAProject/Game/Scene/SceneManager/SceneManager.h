#pragma once
#include"BaseScene/BaseScene.h"

class SceneManager{
public:
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
	void SetNextScene(BaseScene* nextScene) { nextScane_ = nextScene; }

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
	BaseScene* scane_ = nullptr;
	//次のシーン
	BaseScene* nextScane_ = nullptr;

};


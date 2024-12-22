#include "SceneManager.h"
#include<assert.h>

void SceneManager::ChangeScene(const AbstractSceneFactory::SceneName nextScene){
	assert(sceneFactory_);
	assert(nextScane_ == nullptr);

	//次のシーンを生成
	nextScane_ = sceneFactory_->CreateScene(nextScene);
}

void SceneManager::Update(){
	/*シーン切り替え処理*/
	if (nextScane_){
		//旧シーンの終了
		if (scane_){
			scane_->Finalize();
		}
		//シーン切り替え
		scane_ = std::move(nextScane_);
		nextScane_ = nullptr;
		scane_->SetSceneManager(this);
		//次のシーンを初期化する
		scane_->Initialize();
	}

	//実行中のシーンを更新する
	scane_->Update();

}

void SceneManager::Draw3D(){
	scane_->AllDraw3D();
}

void SceneManager::Draw2D(){
	scane_->AllDraw2D();
}

void SceneManager::Finalize(){
	scane_->Finalize();
}

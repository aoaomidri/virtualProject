#include "SceneManager.h"

void SceneManager::Update(){
	/*シーン切り替え処理*/
	if (nextScane_){
		//旧シーンの終了
		if (scane_){
			scane_->Finalize();
			delete scane_;

		}

		//シーン切り替え
		scane_ = nextScane_;
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
	delete scane_;
}

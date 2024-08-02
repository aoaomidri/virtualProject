#include "MAFramework.h"

void MAFramework::Initialize(){
	window_ = WinApp::GetInstance();
	window_->Initialize();

	/*キー入力の初期化処理*/
	input_ = Input::GetInstance();
	input_->Initialize();

	dxCom_ = DirectXCommon::GetInstance();
	dxCom_->Initialize();

	randomMaker_ = RandomMaker::GetInstance();
	//此処だけに初期化
	randomMaker_->Initialize();

	audio_ = Audio::GetInstance();
	audio_->Initialize();

	texManager_ = TextureManager::GetInstance();
	texManager_->Initialize();

	computeManager_ = ComputePipeLineManager::GetInstance();
	computeManager_->Initialize();

	postEffect_ = PostEffect::GetInstance();
	postEffect_->Initialize();
}

void MAFramework::Finalize(){
	CoUninitialize();

	Model::GetInstance()->Finalize();

	audio_->Finalize();

	window_->Finalize();
}

void MAFramework::Update(){
	//Windowにメッセージが来てたら最優先で処理させる
	if (window_->ProcessMessage()) {
		endRequst_ = true;
	}

	input_->Update();
}

void MAFramework::Run(){
	//初期化
	Initialize();
	while (msg.message != WM_QUIT){
		Update();
		if (IsEndRequst()){
			break;
		}
		Draw();
	}

	Finalize();
}

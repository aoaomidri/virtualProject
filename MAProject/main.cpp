#include"ResourceObject.h"
#include"WholeGame.h"
/*メインアプリケーションの起動*/

//解放漏れのチェック
static ResourceObject leakCheck;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	std::unique_ptr<MAFramework> game_ = std::make_unique<WholeGame>();
	
	game_->Run();

	return 0;
}


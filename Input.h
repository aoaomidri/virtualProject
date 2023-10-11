#pragma once
#include<Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION  0x0800
#include<dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input
{
public:
	Input();

	//初期化
	void Initialize(WNDCLASS wc, HWND hwnd);
	//更新
	void Update();


	bool Pushkey(BYTE keyNumber);

	bool Trigerkey(BYTE keyNumber);

	bool Relecekey(BYTE keyNumber);


	//→
	bool PushRight();
	bool TrigerRight();
	//←
	bool PushLeft();
	bool TrigerLeft();
	//↑
	bool PushUp();
	bool TrigerUp();
	//↓
	bool PushDown();
	bool TrigerDown();

	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	BYTE key[256] = {};
	BYTE prekey[256] = {};

	ComPtr<IDirectInputDevice8> keyboard;

	ComPtr<IDirectInput8> directInput;

};


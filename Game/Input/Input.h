#pragma once
#include"WinApp.h"
#include<wrl.h>
#define DIRECTINPUT_VERSION  0x0800
#include<dinput.h>
#include<Xinput.h>
#include"Vector2.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")


class Input
{
private:
	Input() = default;
	~Input() = default;
	Input(const Input& input) = delete;
	Input& operator=(const Input&) = delete;
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// これにより1つしか生成されなくなる
	/// </summary>
	/// <returns>KeyInputのシングルトンインスタンス</returns>
	static Input* GetInstance();

	//初期化
	void Initialize();
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

	//コントローラー
	bool GetPadButton(UINT button);

	bool GetPadButtonRelease(UINT button);

	bool GetPadButtonTriger(UINT button);

	Vector2 GetPadLStick();

	Vector2 GetPadRStick();

	bool GetLTriggerDown();

	bool GetRTriggerDown();

	bool GetLTrigger();

	bool GetRTrigger();

	bool GetConnectPad() { return isConnectPad; }

private:
	BYTE key[256] = {};
	BYTE prekey[256] = {};

	bool isConnectPad = false;

	WinApp* winapp_ = nullptr;

	XINPUT_STATE xinputState;

	XINPUT_STATE oldXInputState;

	ComPtr<IDirectInputDevice8> keyboard;

	ComPtr<IDirectInput8> directInput;

};


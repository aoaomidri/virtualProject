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
public:
	Input() = default;
	~Input() = default;
	Input(const Input& input) = delete;
	Input& operator=(const Input&) = delete;


public:
	enum GamePad {
		A = XINPUT_GAMEPAD_A,
		B = XINPUT_GAMEPAD_B,
		X = XINPUT_GAMEPAD_X,
		Y = XINPUT_GAMEPAD_Y,
		LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
		LT,
		RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		RT,
		UP = XINPUT_GAMEPAD_DPAD_UP,//十字上
		DOWN = XINPUT_GAMEPAD_DPAD_DOWN,//十字下
		RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,//十字右
		LEFT = XINPUT_GAMEPAD_DPAD_LEFT,//十字左
		START = XINPUT_GAMEPAD_START,
		BACK = XINPUT_GAMEPAD_BACK,
		L_THUMB = XINPUT_GAMEPAD_RIGHT_THUMB,//Lスティック押し込み
		R_THUMB = XINPUT_GAMEPAD_LEFT_THUMB,//Rスティック押し込み
	};

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

	bool GetConnectPad() const { return isConnectPad; }

private:
	BYTE key[256] = {};
	BYTE prekey[256] = {};

	const float DeadZone_ = 2.0f;

	bool isConnectPad = false;

	WinApp* winapp_ = nullptr;

	XINPUT_STATE xinputState;

	XINPUT_STATE oldXInputState;

	ComPtr<IDirectInputDevice8> keyboard;

	ComPtr<IDirectInput8> directInput;

};

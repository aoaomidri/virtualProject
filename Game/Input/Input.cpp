#include "Input.h"

#include<cassert>

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}


void Input::Initialize() {
	this->winapp_ = WinApp::GetInstance();

	HRESULT hr;

	/*キー入力の初期化処理*/	
	hr = DirectInput8Create
	(this->winapp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの作成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel
	(this->winapp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	ZeroMemory(&xinputState, sizeof(XINPUT_STATE));

	DWORD Dr = XInputGetState(0, &xinputState);

	hr == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
}

void Input::Update() {
	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力状態を取得する
	memcpy(prekey, key, sizeof(key));
	keyboard->GetDeviceState(sizeof(key), key);

	oldXInputState = xinputState;
	DWORD dresult = XInputGetState(0, &xinputState);
	//	接続状況の確認
	dresult == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
	if (isConnectPad) {

		// デッドzoneの設定
		if ((xinputState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_&&
			xinputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_) &&
			(xinputState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_ &&
				xinputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_))
		{
			xinputState.Gamepad.sThumbLX = 0;
			xinputState.Gamepad.sThumbLY = 0;
		}

		if ((xinputState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_ &&
			xinputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_) &&
			(xinputState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_ &&
				xinputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_))
		{
			xinputState.Gamepad.sThumbRX = 0;
			xinputState.Gamepad.sThumbRY = 0;
		}
	}

}

bool Input::Pushkey(BYTE keyNumber) {
	if (key[keyNumber]){
		return true;
	}

	return false;
}

bool Input::Trigerkey(BYTE keyNumber) {
	if (key[keyNumber] && !prekey[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::Relecekey(BYTE keyNumber) {
	if (!key[keyNumber] && prekey[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::PushRight() {
	if (key[DIK_RIGHT]) {
		return true;
	}

	return false;
}

bool Input::TrigerRight() {
	if (key[DIK_RIGHT] && !prekey[DIK_RIGHT]) {
		return true;
	}

	return false;
}

bool Input::PushLeft() {
	if (key[DIK_LEFT]) {
		return true;
	}

	return false;
}

bool Input::TrigerLeft() {
	if (key[DIK_LEFT] && !prekey[DIK_LEFT]) {
		return true;
	}

	return false;
}

bool Input::PushUp() {
	if (key[DIK_UP]) {
		return true;
	}

	return false;
}

bool Input::TrigerUp() {
	if (key[DIK_UP] && !prekey[DIK_UP]) {
		return true;
	}

	return false;
}

bool Input::PushDown() {
	if (key[DIK_DOWN]) {
		return true;
	}

	return false;
}

bool Input::TrigerDown() {
	if (key[DIK_DOWN] && !prekey[DIK_DOWN]) {
		return true;
	}

	return false;
}


bool Input::GetPadButton(UINT button)
{
	return xinputState.Gamepad.wButtons == button;
}

bool Input::GetPadButtonRelease(UINT button)
{
	return xinputState.Gamepad.wButtons != button && oldXInputState.Gamepad.wButtons == button;
}

bool Input::GetPadButtonTriger(UINT button)
{
	return xinputState.Gamepad.wButtons == button && oldXInputState.Gamepad.wButtons != button;
}

Vector2 Input::GetPadLStick()
{
	SHORT x = xinputState.Gamepad.sThumbLX;
	SHORT y = xinputState.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 Input::GetPadRStick()
{
	SHORT x = xinputState.Gamepad.sThumbRX;
	SHORT y = xinputState.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

bool Input::GetLTriggerDown()
{
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bLeftTrigger < 128 && xinputState.Gamepad.bLeftTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool Input::GetRTriggerDown()
{
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bRightTrigger < 128 && xinputState.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}
bool Input::GetLTrigger()
{
	//	デッドラインの設定必須
	if (xinputState.Gamepad.bLeftTrigger >= 128) {
		return true;
	}
	return false;
}

bool Input::GetRTrigger()
{
	//	デッドラインの設定必須
	if (xinputState.Gamepad.bRightTrigger >= 128) {
		return true;
	}
	return false;
}



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
	(this->winapp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの作成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel
	(this->winapp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	ZeroMemory(&xinputState_, sizeof(XINPUT_STATE));

	hr == ERROR_SUCCESS ? isConnectPad_ = true : isConnectPad_ = false;
}

void Input::Update() {
	//キーボード情報の取得開始
	keyboard_->Acquire();
	//全キーの入力状態を取得する
	memcpy(prekey_, key_, sizeof(key_));
	keyboard_->GetDeviceState(sizeof(key_), key_);

	oldXInputState_ = xinputState_;
	DWORD dresult = XInputGetState(0, &xinputState_);
	//	接続状況の確認
	dresult == ERROR_SUCCESS ? isConnectPad_ = true : isConnectPad_ = false;
	if (isConnectPad_) {

		// デッドzoneの設定
		if ((xinputState_.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_&&
			xinputState_.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_) &&
			(xinputState_.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_ &&
				xinputState_.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * DeadZone_))
		{
			xinputState_.Gamepad.sThumbLX = 0;
			xinputState_.Gamepad.sThumbLY = 0;
		}

		if ((xinputState_.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_ &&
			xinputState_.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_) &&
			(xinputState_.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_ &&
				xinputState_.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * DeadZone_))
		{
			xinputState_.Gamepad.sThumbRX = 0;
			xinputState_.Gamepad.sThumbRY = 0;
		}
	}

}

bool Input::Pushkey(BYTE keyNumber) {
	if (key_[keyNumber]){
		return true;
	}

	return false;
}

bool Input::Trigerkey(BYTE keyNumber) {
	if (key_[keyNumber] && !prekey_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::Relecekey(BYTE keyNumber) {
	if (!key_[keyNumber] && prekey_[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::PushRight() {
	if (key_[DIK_RIGHT]) {
		return true;
	}

	return false;
}

bool Input::TrigerRight() {
	if (key_[DIK_RIGHT] && !prekey_[DIK_RIGHT]) {
		return true;
	}

	return false;
}

bool Input::PushLeft() {
	if (key_[DIK_LEFT]) {
		return true;
	}

	return false;
}

bool Input::TrigerLeft() {
	if (key_[DIK_LEFT] && !prekey_[DIK_LEFT]) {
		return true;
	}

	return false;
}

bool Input::PushUp() {
	if (key_[DIK_UP]) {
		return true;
	}

	return false;
}

bool Input::TrigerUp() {
	if (key_[DIK_UP] && !prekey_[DIK_UP]) {
		return true;
	}

	return false;
}

bool Input::PushDown() {
	if (key_[DIK_DOWN]) {
		return true;
	}

	return false;
}

bool Input::TrigerDown() {
	if (key_[DIK_DOWN] && !prekey_[DIK_DOWN]) {
		return true;
	}

	return false;
}


bool Input::GetPadButton(UINT button){
	if (button == GamePad::LT) {
		return GetLTrigger();
	}

	if (button == GamePad::RT) {
		return GetRTrigger();
	}

	return xinputState_.Gamepad.wButtons == button;
}

bool Input::GetPadButtonRelease(UINT button)
{
	return xinputState_.Gamepad.wButtons != button && oldXInputState_.Gamepad.wButtons == button;
}

bool Input::GetPadButtonTriger(UINT button){
	if (button == GamePad::LT) {
		return GetLTriggerDown();
	}

	if (button == GamePad::RT) {
		return GetRTriggerDown();
	}

	return xinputState_.Gamepad.wButtons == button && oldXInputState_.Gamepad.wButtons != button;
}

bool Input::GetIsPushedLStick() const{

	float x = static_cast<float>(xinputState_.Gamepad.sThumbLX);
	float y = static_cast<float>(xinputState_.Gamepad.sThumbLY);

	if (x != 0.000f or y != 0.000f){
		return true;
	}
	else {
		return false;
	}

}

Vector2 Input::GetPadLStick() const
{
	SHORT x = xinputState_.Gamepad.sThumbLX;
	SHORT y = xinputState_.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 Input::GetPadRStick() const
{
	SHORT x = xinputState_.Gamepad.sThumbRX;
	SHORT y = xinputState_.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

bool Input::GetLTriggerDown()
{
	//	デッドラインの設定必須
	if (oldXInputState_.Gamepad.bLeftTrigger < 128 && xinputState_.Gamepad.bLeftTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool Input::GetRTriggerDown()
{
	//	デッドラインの設定必須
	if (oldXInputState_.Gamepad.bRightTrigger < 128 && xinputState_.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}
bool Input::GetLTrigger()
{
	//	デッドラインの設定必須
	if (xinputState_.Gamepad.bLeftTrigger >= 128) {
		return true;
	}
	return false;
}

bool Input::GetRTrigger()
{
	//	デッドラインの設定必須
	if (xinputState_.Gamepad.bRightTrigger >= 128) {
		return true;
	}
	return false;
}



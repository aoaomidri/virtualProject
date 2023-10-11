#include "Input.h"

#include<cassert>

Input::Input() {
	keyboard = nullptr;
	directInput = nullptr;
	
}


void Input::Initialize(WinApp* winapp) {
	this->winapp_ = winapp;

	HRESULT hr;

	/*キー入力の初期化処理*/	
	hr = DirectInput8Create
	(winapp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの作成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel
	(winapp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update() {
	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力状態を取得する
	memcpy(prekey, key, sizeof(key));
	keyboard->GetDeviceState(sizeof(key), key);

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




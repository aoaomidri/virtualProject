#pragma once
class DirectXCommon
{
public:
	//初期化
	void Initialize();

private:
	//デバイスの初期化
	void DeviceInitialize();

	//コマンド関連の初期化
	void CommandInitialize();

	//スワップチェーン関連の初期化
	void SwapchainInitialize();

	//RTV関連の初期化
	void RTVInitialize();

	//深度バッファ関連の初期化
	void DepthBufferInitialize();

	//フェンスの初期化
	void FenceInitialize();

};


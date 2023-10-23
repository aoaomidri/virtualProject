#pragma once
#include "math/Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"Log.h"

/// <summary>
/// スプライト
/// </summary>
/// 


class Sprite {
public:
	~Sprite();

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle);

	void SetPosition(const Vector2& position) { position_ = position; }

	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	const Vector2& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeSpriteResource();
private:
	HRESULT hr;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSprite;

	//データを書き込む
	Matrix4x4* wvpDataSprite = nullptr;

	Vector2 position_ = { 0.0f,0.0f };

	Transform transformSprite{};

	Transform cameraTransform{};

	bool isDraw_ = true;

	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
};


#pragma once
#include "math/Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"GraphicsPipeline.h"
#include"Transform.h"

/// <summary>
/// スプライト
/// </summary>
/// 
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};


class Sprite {
public:
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeSpriteResource();
private:
	HRESULT hr;

	std::unique_ptr<GraphicsPipeline>GraphicsPipeline_;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//データを書き込む
	Matrix4x4* wvpData = nullptr;

	Transform transform{};

	Transform cameraTransform{};

	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
};


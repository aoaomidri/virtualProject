#pragma once
#include "math/Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"TextureManager.h"
#include"Log.h"

/// <summary>
/// スプライト
/// </summary>
/// 
enum VertexNumber{
	LB,//左下
	LT,//左上
	RB,//右下
	RT //右上
};

class Sprite {
public:
	Sprite();
	~Sprite();

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	void Update();

	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle);

	/*ゲッター群*/

	const Vector2& GetPosition()const { return position_; }

	float GetRotation() const { return rotation_; }

	const Vector2& GetScale() const { return scale_; }

	const Vector2& GetAnchorPoint() const { return anchorPoint_; }

	const Vector4& GetColor()const { return color_; }

	const bool& GetIsDraw()const { return isDraw_; }

private:
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

	//マテリアルにデータを書き込む
	Matrix4x4* wvpDataSprite = nullptr;

	Vector4* materialDate = nullptr;

	VertexData* vertexDataSprite = nullptr;

	Transform transformSprite{};

	Transform cameraTransform{};

	TextureManager* textureManager_ = nullptr;

public:
	//データを書き込む
	Vector2 position_ = { 0.0f,0.0f };

	float rotation_;

	Vector2 scale_ = { 640.0f,360.0f };

	Vector2 anchorPoint_ = { 0.0f,0.0f };

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	bool isDraw_ = true;
};


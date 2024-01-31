#pragma once
#include "Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include <memory>
#include"Model.h"
#include"TextureManager.h"

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
	Sprite() = default;
	~Sprite() = default;

	void Initialize(uint32_t TextureNumber = UINT32_MAX);

	void Update();

	void Draw();

	///*Setter*///
	void SetPosition(const Vector2& position) { position_ = position; }

	void SetRotation(float rotation) { rotation_ = rotation; }

	void SetScale(const Vector2& scale) { scale_ = scale; }

	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

	void SetColor(const Vector4& color) { color_ = color; }

	void SetSize(const Vector2& size) { textureSize_ = size; }

	void SetUVTransform(const Transform& uvTransform) { uvTransform_ = uvTransform; }

	void SetIsDraw(bool isDraw) { isDraw_ = isDraw; }

	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }

	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	void SetLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }


	///*Getter*///
	const Vector2& GetPosition()const { return position_; }

	float GetRotation() const { return rotation_; }

	const Vector2& GetScale() const { return scale_; }

	const Vector2& GetAnchorPoint() const { return anchorPoint_; }

	const Vector4& GetColor()const { return color_; }

	const Vector2& GetLeftTop()const { return textureLeftTop_; }

	const Vector2& GetSize()const { return textureSize_; }

	const Transform& GetUVTransform()const { return uvTransform_; }

	bool GetIsDraw() { return isDraw_; }

	bool GetIsFlipX() {return isFlipX_ ; }
	
	bool GetIsFlipY() { return isFlipY_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeSpriteResource();

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
private:

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
	Material* materialDate = nullptr;

	VertexData* vertexDataSprite = nullptr;

	Matrix4x4* wvpDataSprite = nullptr;

	TextureManager* textureManager_ = nullptr;

	Transform transformSprite{};

	Transform cameraTransform{};

	int textureNumber_;

public:
	//データを書き込む

	Vector2 position_ = { 0.0f,0.0f };

	float rotation_;

	Vector2 scale_ = { 100.0f,100.0f };

	Vector2 anchorPoint_ = { 0.0f,0.0f };

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };	

	//テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };

	Transform uvTransform_{};
	//描画設定
	bool isDraw_ = true;
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;
};


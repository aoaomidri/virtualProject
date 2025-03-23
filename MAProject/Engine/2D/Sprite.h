#pragma once
#include "Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include <memory>
#include"Model.h"
#include"TextureManager.h"
/*2Dテクスチャの描画*/

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
	//初期化処理
	void Initialize(uint32_t TextureNumber = UINT32_MAX);	
	//画面前方に表示するスプライトの描画
	void Draw();
	//3D空間に表示するスプライトの描画
	void Draw(const Matrix4x4& viewPro);

	///*Setter*///
	void SetPosition(const Vector3& position) { position_ = position; }

	void SetRotation(const Vector3& rotation) { rotation_ = rotation; }

	void SetScale(const Vector2& scale) { scale_ = scale; }

	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

	void SetColor(const Vector4& color) { color_ = color; }

	void SetSize(const Vector2& size) { textureSize_ = size; }

	void SetUVTransform(const EulerTransform& uvTransform) { uvTransform_ = uvTransform; }

	void SetIsDraw(bool isDraw) { isDraw_ = isDraw; }

	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }

	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	void SetLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }

	void SetTextureNumber(const uint32_t textureNumber) { 
		textureNumber_ = textureNumber; 
		AdjustTextureSize();
		//テクスチャサイズをスプライトのサイズに適用
		scale_ = textureSize_;
	}


	///*Getter*///
	const Vector3& GetPosition()const { return position_; }

	const Vector3& GetRotation() const { return rotation_; }

	const Vector2& GetScale() const { return scale_; }

	const Vector2& GetAnchorPoint() const { return anchorPoint_; }

	const Vector4& GetColor()const { return color_; }

	const Vector2& GetLeftTop()const { return textureLeftTop_; }

	const Vector2& GetSize()const { return textureSize_; }

	const EulerTransform& GetUVTransform()const { return uvTransform_; }

	bool GetIsDraw() const { return isDraw_; }

	bool GetIsFlipX() const {return isFlipX_ ; }
	
	bool GetIsFlipY() const { return isFlipY_; }
	//Resourceを生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);
	//Resourceづくりをまとめる
	void makeSpriteResource();

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
private:
	//更新処理
	void Update();
	
	//3d空間上に設置するための更新処理
	void Update(const Matrix4x4& viewPro);

private:
	HRESULT hr_{};

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite_;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite_;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSprite_;

	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	VertexData* vertexDataSprite_ = nullptr;

	Matrix4x4* wvpDataSprite_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	EulerTransform transformSprite_{};

	EulerTransform cameraTransform_{};

	int textureNumber_ = 0;

public:
	//データを書き込む

	Vector3 position_ = { 0.0f,0.0f };

	Vector3 rotation_{};

	Vector2 scale_ = { 100.0f,100.0f };

	Vector2 anchorPoint_ = { 0.0f,0.0f };

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };	

	//テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ右側に補正を掛け伸ばす
	Vector2 textureAddRightPos_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };

	EulerTransform uvTransform_{};
	//描画設定
	bool isDraw_ = true;
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;
};


#pragma once
#include "Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"ViewProjection.h"
#include"RandomMaker.h"
#include"GraphicsPipeline.h"
#include"TextureManager.h"

class TrailEffect{
public:

	//位置を保存する構造体
	struct PosBuffer
	{
		Vector3 head; //先端の位置
		Vector3 tail; //末端の位置
	};

	//頂点バッファ
	struct SwordTrailVertex
	{
		Vector3 pos = { 0, 0, 0 };
		Vector2 uv = { 0, 0 };
	};
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="bufferSize">vectorコンテナのサイズの設定 </param>
	/// <param name="texturePath">テクスチャの指定 </param>
	void Initialize(const int bufferSize, const std::string& texturePath);
	//更新処理
	void Update();
	/// <summary>
	/// 現在のフレームの座標を保存する
	/// </summary>
	/// <param name="head">先端</param>
	/// <param name="tail">末端</param>
	void SetPos(const Vector3& head,const Vector3& tail) {
		tempPos_.head = head;
		tempPos_.tail = tail;
	};

	void Reset() {
		posArray_.clear();
		vertex_.clear();
		indices_.clear();
		posArray_.resize(maxSegment_);
	}

	void Draw() const;

	void DrawImgui(std::string name);

	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBuffer() const{
		return &vertexBufferView_;
	}

	const D3D12_INDEX_BUFFER_VIEW* GetIndexBuffer() const {
		return &indexBufferView_;
	}

	const uint32_t GetVertexSize() const {
		return static_cast<uint32_t>(vertex_.size());
	}

	const uint32_t GetIndexSize() const {
		return static_cast<uint32_t>(indices_.size());
	}

	const uint32_t GetTextureHandle() const {
		return textureHandle_;
	}

	const uint32_t GetMax()const {
		return max_;
	}

	const size_t GetBufferSize() const{
		return bufferSize_;
	}
private:
	std::vector<PosBuffer>GetUsedPosArray();
	//曲線で補正を掛けて滑らかにする
	void CreateCurveVertex(std::vector<PosBuffer>& container);

	void MakeVertexData();
	
private:

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate_ = nullptr;

	//頂点indexバッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//頂点リソースにデータを書き込む
	uint32_t* indexData_ = nullptr;

	//トレイルに関する変数
	//位置を保存するバッファ
	std::vector<PosBuffer> posArray_;
	//頂点バッファ
	std::vector<VertexData> vertex_;

	//頂点バッファ
	std::vector<uint32_t> indices_;

	size_t vertexNum_ = 0;

	size_t indexNum_ = 0;

	//分割数
	uint32_t divisionNumber_ = 4;

	//最大描画数
	uint32_t maxSegment_;

	uint32_t max_;

	uint32_t indexCount_;

	//現在の座標
	PosBuffer tempPos_;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//利用しているバッファの数
	size_t bufferSize_ = 0;
};


#pragma once
#include"Matrix.h"
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"TextureManager.h"
#include"Model.h"
#include"DirectionalLight.h"
#include"ViewProjection.h"

class SkyBox{
public:
	struct CameraForGPU {
		Vector3 worldPosition;
	};

	struct SkyBoxVertexData{
		Vector4 position;
	};

	void Initialize(const std::string fileName);

	void Update(const ViewProjection& viewProjection);

	void Draw();

	void DrawImgui(std::string name);

	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	const bool& GetIsDraw()const { return isDraw_; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void MakeVertexResouce();

	void makeResource();

public:
	Matrix4x4* parent_{};

private:
	uint32_t texHandle_;

	HRESULT hr;

	const size_t kVertexNum_ = 8;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソースにデータを書き込む
	SkyBoxVertexData* vertexDate_ = nullptr;

	const uint32_t kIndexNum_ = 36;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//頂点リソースにデータを書き込む
	uint32_t* indexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Model::Material* materialDate = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//データを書き込む
	TransformationMatrix* wvpData = nullptr;

	/*ライト関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;


	//データを書き込む
	//アニメーションの再生中の時刻

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Matrix4x4 localMatrix_;

	bool isUseLight_ = false;

	bool isDraw_ = true;

public:
	//反射強度
	float shininess_ = 1.0f;

	EulerTransform transform_;


};


#pragma once
#include"externals/DirectXTex/d3dx12.h"
#include"externals/DirectXTex/DirectXTex.h"
#include <string>
#include <wrl.h>
#include "math/Matrix.h"
#include"Log.h"

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager() = default;

	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	//	DirectX12のTextureResourceを作る
	//	DirectX12のTextureResourceを作る
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages,
		ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);

	//	オブジェクトファイルを読み込む関数
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//	デスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

private:
	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

};
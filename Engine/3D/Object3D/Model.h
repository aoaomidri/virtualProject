#pragma once
#include"Log.h"
#include"Transform.h"
#include<sstream>
#include<vector>
#include <Windows.h>
#include <wrl.h>
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"
#include"Quaternion.h"
#include<map>
#include<optional>
#include<span>
#include<array>


class Model{
public:
	struct MaterialData {
		std::string textureFilePath;
	};

	struct Node {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
	};

	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	struct DirectionalLight {
		Vector4 color;		//ライトの色
		Vector3 direction;	//ライトの向き
		float intensity;	//輝度
	};

	struct PointLight {
		Vector4 color;		//ライトの色
		Vector3 position;	//ライトの位置
		float intensity;	//輝度
		float radius;		//ライトの届く最大距離
		float decay;		//減衰率
		float padding[2];
	};

	struct SpotLight {
		Vector4 color;
		Vector3 position;
		float intensity;
	};

	//struct KeyframeVector3 {
	//	Vector3 value;//キーフレームの値
	//	float time;//キーフレームの時刻(単位は別)
	//};

	//struct KeyframeQuaternion {
	//	Quaternion value;//キーフレームの値
	//	float time;//キーフレームの時刻(単位は別)
	//};

	template <typename tValue>

	struct Keyframe{
		float time;//キーフレームの時刻(単位は別)
		tValue value;
	};

	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	struct NodeAnimation {
		std::vector<KeyframeVector3> translate;
		std::vector<KeyframeQuaternion> rotate;
		std::vector<KeyframeVector3> scale;
	};

	struct Animation{
		float duration;//アニメーション全体の尺(単位は秒)
		//NodeAnimationの集合、Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
		//アニメーションをループさせるかどうか
		bool isAnimLoop;
	};

	struct Joint {
		QuaternionTransform transform;//Trnasform情報
		Matrix4x4 localMatrix;//localMatrix
		Matrix4x4 skeltonSpaceMatrix;
		std::string name;
		std::vector<int32_t> children;//子JointのIndexのリスト。いなければ空
		int32_t index;//自身のIndex
		std::optional<int32_t> parent;//親JointのIndex。いなければnull
	};

	struct Skeleton{
		int32_t root;
		std::map<std::string, int32_t> jointMap;
		std::vector<Joint> joints;
	};

	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence{
		std::array<float, kNumMaxInfluence> weights;
		std::array<uint32_t, kNumMaxInfluence> jointIndices;
	};

	struct WellForGPU{
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};

	//GPUで扱うことが出来るようにする
	struct SkinCluster{
		//カテゴリ1
		std::vector<Matrix4x4> inverseBindPoseMatrices;

		//カテゴリ2
		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResouce;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;

		//カテゴリ3
		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResouce;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	};

	
public:
	void Draw(ID3D12GraphicsCommandList* CommandList);

	static Model* GetInstance();
	//modelの読み込み
	//ファイルの名前と一致しているならば
	static std::unique_ptr<Model> LoadModelFile(const std::string& filename);
	//ファイルの名前が違うのであれば
	static std::unique_ptr<Model> LoadModelFile(const std::string& filename, const std::string& modelName);
	// ファイルの名前と一致しているならば
	static Model::Animation LoadAnimationFile(const std::string& filename);
	//ファイルの名前が違うのであれば
	static Model::Animation LoadAnimationFile(const std::string& filename, const std::string& modelName);


	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }

	void Finalize();

	Skeleton CreateSkeleton(const Node& rootNode);

	SkinCluster CreateSkinCluster(const Skeleton& skeleton);

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()const { return vertexBufferView_; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBifferView()const { return indexBufferView_; }
	
	Matrix4x4 GetLocalMatrix()const { return modelData_.rootNode.localMatrix; }

	Node GetNode()const { return modelData_.rootNode; }

	std::string GetNodeName()const { return modelData_.rootNode.name; }

	MaterialData GetMaterial()const { return modelData_.material; }

	const std::vector<VertexData> GetVertexData()const { return modelData_.vertices; }

	const std::vector<uint32_t> GetIndexData()const { return modelData_.indices; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal(const std::string& filename);

	void LoadFromOBJInternalAssimp(const std::string& filename, const std::string& modelName);

	void MakeVertexResource();

	Node ReadNode(aiNode* node);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	
private:
	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate_ = nullptr;

	//頂点リソースにデータを書き込む
	uint32_t* mappedIndex_ = nullptr;

	const std::string ResourcesPath_ = "resources/Model/";

	//頂点インデックス配列
	ModelData modelData_;

	static ID3D12Device* device_;

};


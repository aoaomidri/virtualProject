#include "Model.h"
#include"DirectXCommon.h"
#include<cassert>
#include<fstream>
#include<sstream>

//静的メンバ変数の実体
ID3D12Device* Model::device_ = nullptr;

void Model::Draw(ID3D12GraphicsCommandList* CommandList){
	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
}

Model* Model::GetInstance(){
	static Model instance;
	return &instance;
}

std::unique_ptr<Model> Model::LoadObjFile(const std::string& filename){
	//1,中で必要になる変数の宣言
	std::unique_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData
	
	modelData->LoadFromOBJInternalAssimp(filename);

	modelData->MakeVertexResource();

	
	//4,ModelDataを返す
	return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
	//1,中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの	

	//2,ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//とりあえず開けなかったら止める

	//3,実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			std::string result;

			s >> textureFilename;
			size_t slashPos_ = textureFilename.find_last_of('\\');
			size_t dotPos_ = textureFilename.find_last_of('.');
			if (slashPos_>1000){
				//連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + textureFilename;
			}
			else {
				if (slashPos_ != std::string::npos && dotPos_ != std::string::npos && dotPos_ > slashPos_) {
					result = textureFilename.substr(slashPos_ + 1, dotPos_ - slashPos_ - 1);
				}
				//連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + result + ".png";
			}
			
		}

	}
	//4,MaterialDataを返す
	return materialData;
	
}

void Model::Finalize(){
	//device_->Release();
}

Microsoft::WRL::ComPtr<ID3D12Resource> Model::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes){
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

void Model::LoadFromOBJInternal(const std::string& filename){
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの

	//2,ファイルを開く

	std::ifstream file(ResourcesPath_ + filename + "/" + filename + ".obj");//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	//3,実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む
		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position{};
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord{};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal{};
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3]{};
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのindexは「位置/UV/法線」で格納されているので、分解してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);

				}
				//要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position,texcoord,normal };
				indices_.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };

			}
			indices_.push_back(triangle[2]);
			indices_.push_back(triangle[1]);
			indices_.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルに同一改装にmtlは存在させるので、ディレクトリとファイル名を渡す
			material_ = LoadMaterialTemplateFile(ResourcesPath_ + filename, materialFilename);
		}

	}
}

void Model::LoadFromOBJInternalAssimp(const std::string& filename){
	std::string line;//ファイルから読んだ1行を格納するもの

	//2,ファイルを開く
	Assimp::Importer importer;
	std::string filepath = ResourcesPath_ + filename + "/" + filename + ".obj";
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	//3、Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//Texcoordがないメッシュは今回は非対応
		//ここからMeshの中身(face)の解析を行っていく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex){
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート
			//ここからFaceの中身(Vertex)の解析を行っていく
			for (uint32_t element = 0; element < face.mNumIndices; ++element){
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				//aiProcess_MakeLeftHandedはz*=-1で、右手→左手に変換するので手動で対応
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				indices_.push_back(vertex);

			}

		}
	}

	//4,Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex){
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			material_.textureFilePath = ResourcesPath_ + filepath + "/" + textureFilePath.C_Str();
		}
	}

}

void Model::MakeVertexResource(){
	//頂点リソースの作成
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * indices_.size());


	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * indices_.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	std::memcpy(vertexDate_, indices_.data(), sizeof(VertexData) * indices_.size());
}

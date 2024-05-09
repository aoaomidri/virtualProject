#include "Model.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include<cassert>
#include<fstream>
#include<sstream>
#include<filesystem>
#include"Matrix.h"

//静的メンバ変数の実体
ID3D12Device* Model::device_ = nullptr;


void Model::Draw(ID3D12GraphicsCommandList* CommandList){
	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	CommandList->IASetIndexBuffer(&indexBufferView_);
}

Model* Model::GetInstance(){
	static Model instance;
	return &instance;
}

std::unique_ptr<Model> Model::LoadModelFile(const std::string& filename){
	//1,中で必要になる変数の宣言
	std::unique_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData
	
	modelData->LoadFromOBJInternalAssimp(filename, filename);

	modelData->MakeVertexResource();

	//4,ModelDataを返す
	return modelData;
}

std::unique_ptr<Model> Model::LoadModelFile(const std::string& filename, const std::string& modelName){
	//1,中で必要になる変数の宣言
	std::unique_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData

	modelData->LoadFromOBJInternalAssimp(filename, modelName);

	modelData->MakeVertexResource();

	//4,ModelDataを返す
	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
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
				modelData_.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };

			}
			modelData_.vertices.push_back(triangle[2]);
			modelData_.vertices.push_back(triangle[1]);
			modelData_.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルに同一改装にmtlは存在させるので、ディレクトリとファイル名を渡す
			modelData_.material = LoadMaterialTemplateFile(ResourcesPath_ + filename, materialFilename);
		}

	}
}

void Model::LoadFromOBJInternalAssimp(const std::string& filename, const std::string& modelName) {
	//2,ファイルを開く
	Assimp::Importer importer;
	std::string extension;
	std::filesystem::directory_iterator dIterator{ ResourcesPath_ + filename + "/" };
	for (const auto& entry : dIterator){
		const auto& path = entry.path();
		if (path.extension() == ".obj") {
			extension = ".obj";
			break;
		}
		else if (path.extension() == ".gltf") {
			extension = ".gltf";
			break;
		}
	}
	std::filesystem::path filepath = ResourcesPath_ + filename + "/" + modelName + extension;
	const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	//3、Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//Texcoordがないメッシュは今回は非対応
		//ここからMeshの中身(face)の解析を行っていく
		modelData_.vertices.resize(mesh->mNumVertices);//最初に頂点数分のメモリを確保していく
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData_.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData_.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData_.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };

		}
		//Indexを解析していく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex){
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element){
				uint32_t vertexIndex = face.mIndices[element];
				modelData_.indices.push_back(vertexIndex);
			}

		}
		//SkinCluster構築用のデータ取得を追加
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData_.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = 
				Matrix::GetInstance()->MakeAffineMatrix(
					Vector3({ scale.x,scale.y,scale.z }), 
					Quaternion({ rotate.x,-rotate.y,-rotate.z,rotate.w }), 
					Vector3({ -translate.x,translate.y,translate.z }));

			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverce();

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex){
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}

		}

	}

	//4,Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::filesystem::path textureFileTemp = textureFilePath.C_Str();
			modelData_.material.textureFilePath = filepath.parent_path().string() + "/" + textureFileTemp.filename().string();
		}
	}

	//5,Nodeの解析
	

	modelData_.rootNode = ReadNode(scene->mRootNode);

}

void Model::MakeVertexResource(){
	//頂点リソースの作成
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());


	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	std::memcpy(vertexDate_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());


	uint32_t indexSizeInBytes = static_cast<uint32_t>(sizeof(uint32_t) * modelData_.indices.size());

	//インデックスリソースの作成
	indexResource_ = CreateBufferResource(device_, indexSizeInBytes);

	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはindexの数
	indexBufferView_.SizeInBytes = indexSizeInBytes;
	//1頂点当たりのサイズ
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), indexSizeInBytes);
	/*std::copy(modelData_.indices.begin(), modelData_.indices.end(), mappedIndex_);

	indexResource_->Unmap(0, nullptr);*/
}

Model::Node Model::ReadNode(aiNode* node){
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale = { scale.x,scale.y,scale.z };
	result.transform.rotate.quaternion_ = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translate = { -translate.x,translate.y,translate.z };
	result.localMatrix =Matrix::GetInstance()->MakeAffineMatrix(result.transform);
	
	result.name = node->mName.C_Str();//node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++){
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);

	}
	return result;
}

Model::Skeleton Model::CreateSkeleton(const Node& rootNode){
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とIndexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;

}

Model::SkinCluster Model::CreateSkinCluster(const Skeleton& skeleton){
	SkinCluster skinCluster;

	//palette用のResourceを確保
	skinCluster.paletteResouce = CreateBufferResource(device_, sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResouce->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	skinCluster.paletteSrvHandle.first = TextureManager::GetInstance()->GetCPUDescriptorHandle(descriptorSizeSRV, 100);
	skinCluster.paletteSrvHandle.second = TextureManager::GetInstance()->GetGPUDescriptorHandle(descriptorSizeSRV, 100);
	
	//palette用のsrvを作成。StructuredBufferでアクセスできるようにする。
	D3D12_SHADER_RESOURCE_VIEW_DESC paretteSrvDesc{};
	paretteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paretteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paretteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paretteSrvDesc.Buffer.FirstElement = 0;
	paretteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paretteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paretteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device_->CreateShaderResourceView(skinCluster.paletteResouce.Get(), &paretteSrvDesc, skinCluster.paletteSrvHandle.first);
	
	//influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResouce = CreateBufferResource(device_, sizeof(VertexInfluence) * modelData_.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResouce->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData_.vertices.size());//0埋め、weightを0にしておく
	skinCluster.mappedInfluence = { mappedInfluence,modelData_.vertices.size() };

	//influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResouce->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData_.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);
	//InerseBindPoseMatrixの保存領域を作成
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());

	for (size_t i = 0; i < skinCluster.inverseBindPoseMatrices.size(); ++i){
		skinCluster.inverseBindPoseMatrices[i].Identity();
	}
	
	//ModelDataのSkinCluster情報を解析してinfluenceの中身を埋める
	for (const auto& jointWeight : modelData_.skinClusterData){//ModelのSkinClusterの情報を解析
		auto it = skeleton.jointMap.find(jointWeight.first);//jointWeight.firstのJoint名なので、skeletonに対象となるJointが含まれているかどうか判断
		if (it == skeleton.jointMap.end()) {//そんな名前のJointは存在しない。なので次に回す
			continue;
		}

		//(*it).secondにはJointのIndexが入っているので、外套のIndexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights){
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく

			for (uint32_t index = 0; index < kNumMaxInfluence; ++index){//空いているところに入れる
				if (currentInfluence.weights[index] == 0.0f) {//weight==0が空いている状態なので、その場所にweightとJointのIndexを代入
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}

		}

	}

	return skinCluster;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints){
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeltonSpaceMatrix.Identity();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());//現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);//SkeltonのJoint列に追加
	for (const Node& child : node.children) {
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

Model::Animation Model::LoadAnimationFile(const std::string& filename){
	Model::Animation animation;//今回作るアニメーション
	Assimp::Importer importer;
	std::string extension;
	std::filesystem::directory_iterator dIterator{ "resources/Model/" + filename + "/" };
	for (const auto& entry : dIterator) {
		const auto& path = entry.path();
		if (path.extension() == ".obj") {
			extension = ".obj";
			return Model::Animation();
		}
		else if (path.extension() == ".gltf") {
			extension = ".gltf";
			break;
		}
	}
	std::filesystem::path filepath = "resources/Model/" + filename + "/" + filename + extension;

	const aiScene* scene = importer.ReadFile(filepath.string().c_str(), 0);
	if (scene->mNumAnimations == 0){
		return Model::Animation();
	}
	assert(scene->mNumAnimations != 0);//アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションだけ採用。もちろん複数対応するに越したことはない
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換

	//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex){

		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		//positionについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex){
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 pos{};

			pos.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			pos.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手

			nodeAnimation.translate.push_back(pos);
		}

		//rotateについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion rotate{};

			rotate.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			rotate.value.quaternion_ = { keyAssimp.mValue.x,keyAssimp.mValue.y * -1.0f ,keyAssimp.mValue.z * -1.0f ,keyAssimp.mValue.w }; //右手->左手

			nodeAnimation.rotate.push_back(rotate);
		}

		//scaleについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 scale{};

			scale.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			scale.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手

			nodeAnimation.scale.push_back(scale);
		}
	}
	//解析完了
	return animation;
}

Model::Animation Model::LoadAnimationFile(const std::string& filename, const std::string& modelName){
	Model::Animation animation;//今回作るアニメーション
	Assimp::Importer importer;
	std::string extension;
	std::filesystem::directory_iterator dIterator{ "resources/Model/" + filename + "/" };
	for (const auto& entry : dIterator) {
		const auto& path = entry.path();
		if (path.extension() == ".obj") {
			extension = ".obj";
			return Model::Animation();
		}
		else if (path.extension() == ".gltf") {
			extension = ".gltf";
			break;
		}
	}
	std::filesystem::path filepath = "resources/Model/" + filename + "/" + modelName + extension;

	const aiScene* scene = importer.ReadFile(filepath.string().c_str(), 0);
	if (scene->mNumAnimations == 0) {
		return Model::Animation();
	}
	assert(scene->mNumAnimations != 0);//アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションだけ採用。もちろん複数対応するに越したことはない
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換

	//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		//positionについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 pos{};

			pos.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			pos.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手

			nodeAnimation.translate.push_back(pos);
		}

		//rotateについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion rotate{};

			rotate.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			rotate.value.quaternion_ = { keyAssimp.mValue.x,keyAssimp.mValue.y * -1.0f ,keyAssimp.mValue.z * -1.0f ,keyAssimp.mValue.w }; //右手->左手

			nodeAnimation.rotate.push_back(rotate);
		}

		//scaleについて
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 scale{};

			scale.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//此処も秒に変換
			scale.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手

			nodeAnimation.scale.push_back(scale);
		}
	}
	//解析完了
	return animation;
}

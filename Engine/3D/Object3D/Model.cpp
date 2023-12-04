#include "Model.h"
#include<cassert>
#include<fstream>
#include<sstream>

Model* Model::GetInstance(){
	static Model instance;
	return &instance;
}

//Model* Model::LoadObjFile(const std::string& filename){
//
//
//	//1,中で必要になる変数の宣言
//	Model* modelData = new Model();//構築するModelData
//	std::vector<Vector4> positions;//位置
//	std::vector<Vector3> normals;//法線
//	std::vector<Vector2> texcoords;//テクスチャ座標
//	std::string line;//ファイルから読んだ1行を格納するもの
//	const std::string ResourcesPath = "resources/";
//
//	//2,ファイルを開く
//
//	std::ifstream file(ResourcesPath + filename + "/" + filename + ".obj");//ファイルを開く
//	assert(file.is_open());//とりあえず開けなかったら止める
//
//	//3,実際にファイルを読み、ModelDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;//先頭の識別子を読む
//		//identifierに応じた処理
//		if (identifier == "v") {
//			Vector4 position{};
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			position.x *= -1;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt") {
//			Vector2 texcoord{};
//			s >> texcoord.x >> texcoord.y;
//			texcoord.y = 1.0f - texcoord.y;
//			texcoords.push_back(texcoord);
//		}
//		else if (identifier == "vn") {
//			Vector3 normal{};
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1;
//			normals.push_back(normal);
//		}
//		else if (identifier == "f") {
//			VertexData triangle[3]{};
//			//面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのindexは「位置/UV/法線」で格納されているので、分解してindexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3]{};
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//
//				}
//				//要素へのindexから、実際の要素の値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				/*VertexData vertex = { position,texcoord,normal };
//				modelData.vertices.push_back(vertex);*/
//				triangle[faceVertex] = { position,texcoord,normal };
//
//			}
//			indices.push_back(triangle[2]);
//			indices.push_back(triangle[1]);
//			indices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib") {
//			//materialTemplateLibraryファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			//基本的にobjファイルに同一改装にmtlは存在させるので、ディレクトリとファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(ResourcesPath + filename, materialFilename);
//		}
//
//	}
//
//	//4,ModelDataを返す
//	return modelData;
//}
//
//MaterialData* Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
//	//1,中で必要となる変数の宣言
//	MaterialData materialData;//構築するMaterialData
//	std::string line;//ファイルから読んだ1行を格納するもの	
//
//	//2,ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());//とりあえず開けなかったら止める
//
//	//3,実際にファイルを読み、MaterialDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		//identifierに応じた処理
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//
//	}
//	//4,MaterialDataを返す
//	return materialData;
//	
//}

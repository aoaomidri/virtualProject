#include "LevelLoader.h"
#include"Engine/Base/WinApp.h"

LevelLoader* LevelLoader::GetInstance(){
	static LevelLoader instance;
	return &instance;
}

void LevelLoader::LoadLevelData(){
	objects_.clear();
	skinAnimObjects_.clear();

	//連結してフルパスを作る
	const std::string fullpath = kDirectoryPath_ + kfileName_ + kExtexsion_;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン失敗をチェック
	if (file.fail()){
		assert(0);
	}

	//json文字列から解凍したデータ
	json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();

	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	for (json& Jobject : deserialized["object"]) {
		assert(Jobject.contains("type"));
		//種類を取得
		std::string type = Jobject["type"].get< std::string>();

		//MESH
		if (type.compare("MESH") == 0) {
			std::pair<std::string, std::unique_ptr<Object3D>> obj;

			obj.second = std::make_unique<Object3D>();

			if (Jobject.contains("name")){
				//ファイル名
				obj.first = Jobject["name"];
			}

			obj.second->Initialize(Jobject["file_name"]);

			EulerTransform& objectData = obj.second->transform_;


			json& transform = Jobject["transform"];
			//平行移動
			objectData.translate.x = (float)transform["translation"][0];
			objectData.translate.y = (float)transform["translation"][2];
			objectData.translate.z = (float)transform["translation"][1];
			//回転角
			objectData.rotate.x = (float)transform["rotation"][0];
			objectData.rotate.y = (float)transform["rotation"][2];
			objectData.rotate.z = (float)transform["rotation"][1];

			//スケーリング
			objectData.scale.x = (float)transform["scaling"][0];
			objectData.scale.y = (float)transform["scaling"][2];
			objectData.scale.z = (float)transform["scaling"][1];

			objects_.emplace_back(std::move(obj));
		}

	}

}

void LevelLoader::Draw(const ViewProjection& viewProjection){
	for (auto& object : objects_) {
		object.second->Update(viewProjection);
		object.second->Draw();		
	}
}

Object3D* LevelLoader::GetLevelObject(const std::string tag){
	for (auto& object :objects_){
		if (object.first == tag) {
			return object.second.get();
		}
	}

	return nullptr;
}

SkinAnimObject3D* LevelLoader::GetLevelSkinAnimObject(const std::string tag){
	for (auto& object : skinAnimObjects_) {
		if (object.first == tag) {
			return object.second.get();
		}
	}
	return nullptr;
}


void LevelLoader::ChackFiles(){
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::wstring message = L"選択したシーンと一致しませんでした。";
		MessageBoxW(WinApp::GetInstance()->GetHwnd(), message.c_str(), L"シーンが合ってないよぉ！", 0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (fileName_.size() != 0) {
			bool noneFail = true;
			for (size_t i = 0; i < fileName_.size(); i++) {
				if (fileName_[i].c_str() == filePath.stem().string()) {
					noneFail = false;
				}
			}
			if (noneFail) {
				fileName_.push_back(filePath.stem().string());
			}

		}
		else {
			//ファイルの名前を取得
			fileName_.push_back(filePath.stem().string());
		}
	}
}

bool LevelLoader::OperationConfirmation(){

	return false;
}

void LevelLoader::from_json(const json& j, Vector2& v){

}

void LevelLoader::from_json(const json& j, Vector3& v){

}

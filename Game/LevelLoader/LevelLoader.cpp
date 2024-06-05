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

	for (json& object : deserialized["object"]) {
		assert(object.contains("type"));
		//種類を取得
		std::string type = object["type"].get< std::string>();

		//MESH
		if (type.compare("MESH") == 0) {
			objects_.emplace_back(std::pair<std::string, std::unique_ptr<Object3D>>());

			objects_.back().second = std::make_unique<Object3D>();

			std::unique_ptr<Object3D>& objectData = objects_.back().second;

			if (object.contains("file_name")){
				//ファイル名
				objects_.back().first = object["file_name"];
			}

			json& transform = object["transform"];
			//平行移動
			objectData->transform_;
			//回転角

			//スケーリング


		}

	}

}

void LevelLoader::GetLevelObject(){

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

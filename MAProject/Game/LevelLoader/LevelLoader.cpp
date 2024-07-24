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

	LoadJson(deserialized["object"]);

	

}

void LevelLoader::LoadJson(json jData){
	for (json& Jobject : jData) {
		assert(Jobject.contains("type"));
		//種類を取得
		std::string type = Jobject["type"].get< std::string>();

		//MESH
		if (type.compare("MESH") == 0) {
			
			if (Jobject.contains("animation_name")){

				std::pair<std::string, std::unique_ptr<SkinAnimObject3D>> animObj;

				animObj.second = std::make_unique<SkinAnimObject3D>();

				animObj.second->Initialize(Jobject["file_name"], Jobject["animation_name"], true);

				if (Jobject.contains("name")) {
					//ファイル名
					animObj.first = Jobject["name"];
				}

				EulerTransform& objectData = animObj.second->transform_;

				json& transform = Jobject["transform"];
				//平行移動
				objectData.translate.x = (float)transform["translation"][0];
				objectData.translate.y = (float)transform["translation"][2];
				objectData.translate.z = (float)transform["translation"][1];
				//回転角
				objectData.rotate.x = -(float)transform["rotation"][0];
				objectData.rotate.y = -(float)transform["rotation"][2];
				objectData.rotate.z = -(float)transform["rotation"][1];

				//スケーリング
				objectData.scale.x = (float)transform["scaling"][0];
				objectData.scale.y = (float)transform["scaling"][2];
				objectData.scale.z = (float)transform["scaling"][1];

				skinAnimObjects_.emplace_back(std::move(animObj));
				if (Jobject.contains("collider")) {
					std::pair<std::string, OBB> collider;
					collider.first = Jobject["name"];
					//中央
					collider.second.center.x = (float)Jobject["collider"]["center"][0];
					collider.second.center.x = (float)Jobject["collider"]["center"][2];
					collider.second.center.x = (float)Jobject["collider"]["center"][1];

					//スケール
					collider.second.size.x = (float)Jobject["collider"]["size"][0];
					collider.second.size.x = (float)Jobject["collider"]["size"][2];
					collider.second.size.x = (float)Jobject["collider"]["size"][1];
					colliders_.emplace_back(collider);
				}
			}
			else {
				std::pair<std::string, std::unique_ptr<Object3D>> obj;

				obj.second = std::make_unique<Object3D>();

				obj.second->Initialize(Jobject["file_name"]);

				if (Jobject.contains("name")) {
					//ファイル名
					obj.first = Jobject["name"];
				}

				EulerTransform& objectData = obj.second->transform_;

				json& transform = Jobject["transform"];
				//平行移動
				objectData.translate.x = (float)transform["translation"][0];
				objectData.translate.y = (float)transform["translation"][2];
				objectData.translate.z = (float)transform["translation"][1];
				//回転角
				objectData.rotate.x = -(float)transform["rotation"][0];
				objectData.rotate.y = -(float)transform["rotation"][2];
				objectData.rotate.z = -(float)transform["rotation"][1];

				//スケーリング
				objectData.scale.x = (float)transform["scaling"][0];
				objectData.scale.y = (float)transform["scaling"][2];
				objectData.scale.z = (float)transform["scaling"][1];

				objects_.emplace_back(std::move(obj));

				if (Jobject.contains("collider")) {
					std::pair<std::string, OBB> collider;
					collider.first = Jobject["name"];
					//中央
					collider.second.center.x = (float)Jobject["collider"]["center"][0];
					collider.second.center.y = (float)Jobject["collider"]["center"][2];
					collider.second.center.z = (float)Jobject["collider"]["center"][1];

					//スケール
					collider.second.size.x = (float)Jobject["collider"]["size"][0];
					collider.second.size.y = (float)Jobject["collider"]["size"][2];
					collider.second.size.z = (float)Jobject["collider"]["size"][1];
					colliders_.emplace_back(std::move(collider));
				}
			}

			

			
		}
		if (Jobject.contains("children")) {
			LoadJson(Jobject["children"]);
		}
	}
}

//void LevelLoader::Draw(const ViewProjection& viewProjection){
//	for (auto& object : objects_) {
//		object.second->Update(viewProjection);
//		object.second->Draw();		
//	}
//}

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

EulerTransform LevelLoader::GetLevelObjectTransform(const std::string tag){
	for (auto& object : objects_) {
		if (object.first == tag) {
			return object.second.get()->GetTransform();
		}
	}
	return EulerTransform();
}

OBB LevelLoader::GetLevelObjectOBB(const std::string tag){
	for (auto& collider : colliders_) {
		if (collider.first == tag) {
			return collider.second;
		}
	}
	return OBB();
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

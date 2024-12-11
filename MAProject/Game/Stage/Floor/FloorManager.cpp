#include "FloorManager.h"
#include"DirectXCommon.h"

FloorManager::~FloorManager(){
	
}

void FloorManager::Initialize() {
	
}

void FloorManager::Update(){
	
	for (auto it = floors_.begin(); it != floors_.end(); ++it) {
		(*it)->Update();
	}
	
}

void FloorManager::Draw(const ViewProjection& viewProjection){

	for (auto it = floors_.begin(); it != floors_.end(); ++it) {
		(*it)->Draw(viewProjection);
	}

}

void FloorManager::AddFloor(const EulerTransform& transform){
	//std::unique_ptr<Object3D> object_ = std::make_unique<Object3D>();
	//object_->Initialize("Floor");
	//objects_.emplace_back(std::move(object_));

	std::unique_ptr<Floor> floor = std::make_unique<Floor>();
	

	floor->Initialize(transform);

	floors_.emplace_back(std::move(floor));
}

void FloorManager::DrawImgui() {
	int i = 0;

	for (auto it = floors_.begin(); it != floors_.end(); ++it) {
		(*it)->DrawImgui();	
		i++;
	}
}

void FloorManager::SaveFile(const std::vector<std::string>& stages) {
	//保存
	json root;
	root = json::object();
	root[kItemName_] = json::object();

	for (size_t j = 0; j < stages.size(); ++j) {
		int i = 0;
		for (auto it = floors_.begin(); it != floors_.end(); ++it) {
			root[kItemName_][stages[j].c_str()][i][0] = json::array(
				{ (*it)->GetTransform().scale.x,
				  (*it)->GetTransform().scale.y,
				  (*it)->GetTransform().scale.z
				});
			root[kItemName_][stages[j].c_str()][i][1] = json::array(
				{ (*it)->GetTransform().rotate.x,
				  (*it)->GetTransform().rotate.y,
				  (*it)->GetTransform().rotate.z
				});
			root[kItemName_][stages[j].c_str()][i][2] = json::array(
				{ (*it)->GetTransform().translate.x,
				  (*it)->GetTransform().translate.y,
				  (*it)->GetTransform().translate.z
				});
			std::filesystem::path dir(kDirectoryPath_);
			if (!std::filesystem::exists(kDirectoryName_)) {
				std::filesystem::create_directory(kDirectoryName_);
			}
			// 書き込むjsonファイルのフルパスを合成する
			std::string filePath = kDirectoryPath_ + kItemName_ + ".json";
			// 書き込み用ファイルストリーム
			std::ofstream ofs;
			// ファイルを書き込みように開く
			ofs.open(filePath);
			//ファイルオープン失敗
			if (ofs.fail()) {
				std::string message = "Failed open data file for write.";
				MessageBoxA(nullptr, message.c_str(), "Element", 0);
				assert(0);
				break;
			}
			//ファイルにjson文字列を書き込む(インデント幅4)
			ofs << std::setw(4) << root << std::endl;
			//ファイルを閉じる
			ofs.close();
			i++;
		}
	}
}

void FloorManager::FileOverWrite(const std::string& stage) {
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + kItemName_ + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//上書き用に読み取り
	json root;
	ifs >> root;
	ifs.close();

	json overWrite;

	int i = 0;
	for (auto it = floors_.begin(); it != floors_.end(); ++it) {
		overWrite[i][0] = json::array(
			{ (*it)->GetTransform().scale.x,
			  (*it)->GetTransform().scale.y,
			  (*it)->GetTransform().scale.z
			});
		overWrite[i][1] = json::array(
			{ (*it)->GetTransform().rotate.x,
			  (*it)->GetTransform().rotate.y,
			  (*it)->GetTransform().rotate.z
			});
		overWrite[i][2] = json::array(
			{ (*it)->GetTransform().translate.x,
			  (*it)->GetTransform().translate.y,
			  (*it)->GetTransform().translate.z
			});
		overWrite[i][3] = (*it)->GetIsMove();
		i++;			  
	}					  

	root[kItemName_][stage] = overWrite;

	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込みように開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Element", 0);
		assert(0);
		return;
	}
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

void FloorManager::ChackFiles() {
	if (!std::filesystem::exists(kDirectoryPath_)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Box", 0);
		assert(0);
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

		if (LoadChackItem(filePath.stem().string())) {
			chackOnlyNumber_ = 1;
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

void FloorManager::LoadFiles(const std::string& stage) {

	if (!LoadChackItem(kItemName_))
		return;
	if (!std::filesystem::exists(kDirectoryPath_)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Animetion", 0);
		assert(0);
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

		if (filePath.stem().string() == kItemName_) {
			//ファイル読み込み
			LoadFile(filePath.stem().string(), stage);
			return;
		}


	}
}

void FloorManager::LoadFile(const std::string& groupName, const std::string& stage) {
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + groupName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();


	//グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	//未登録チェック
	assert(itGroup != root.end());
	//各アイテムについて
	for (const auto& i : root[groupName][stage]) {
		int count = 0;
		EulerTransform newTrans{};
		bool isNewMove = false;
		for (const auto& j : i) {
			Vector3 v{};

			if (count < 3) {
				from_json(j, v);
			}

			if (count == 0) {
				newTrans.scale = v;
			}
			else if (count == 1) {
				newTrans.rotate = v;
			}
			else if (count == 2) {
				newTrans.translate = v;
			}
			else if (count == 3) {
				isNewMove = j;
			}
			
			count++;

		}


		AddFloor(newTrans);

		
	}

}

bool FloorManager::LoadChackItem(const std::string& itemName) {

	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + itemName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		ifs.close();
		return false;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(itemName);
	//未登録チェック
	if (itGroup != root.end()) {
		return true;
	}
	else {
		return false;
	}
}

void FloorManager::from_json(const json& j, Vector3& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}

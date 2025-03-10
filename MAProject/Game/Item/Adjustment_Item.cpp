#include "Adjustment_Item.h"
#include"../../externals/nlohmann/json.hpp"
#include<fstream>
#include"WinApp.h"

using namespace MAEngine;

Adjustment_Item* Adjustment_Item::GetInstance() {
	static Adjustment_Item inst;
	return &inst;
}

void Adjustment_Item::Update() {
#ifdef _DEBUG
	if (!ImGui::Begin("Adjustment_Item", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;
	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();
		itGroup != datas_.end(); ++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;
		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;
		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.begin();
			itItem != group.end(); ++itItem) {
			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::DragInt(itemName.c_str(), ptr, 1, -9999, 9999);
			}
			//float型の値を保存していれば
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.01f, -2000.0f, 2000.0f);
			}
			//Vector2型の値を保持していれば
			else if (std::holds_alternative<Vector2>(item)) {
				Vector2* ptr = std::get_if<Vector2>(&item);
				ImGui::DragFloat2(
					itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, -2000.0f, 2000.0f);
			}
			//Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(
					itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, -2000.0f                                                                                                                                                                                                                                                                                                                                                                                                                                                                           , 2000.0f);
			}
			//Vector4型の値を保持していれば
			else if (std::holds_alternative<Vector4>(item)) {
				Vector4* ptr = std::get_if<Vector4>(&item);
				ImGui::DragFloat4(
					itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, -100.0f, 100.0f);
			}
		}
		//改行
		ImGui::Text("\n");
		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		}
		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
#endif
}

bool Adjustment_Item::OperationConfirmation(){
	int result = MessageBox(WinApp::GetInstance()->GetHwnd(), L"この操作を続けますか?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES) {
		return true;
	}
	else if (result == IDNO) {
		return false;
	}
	else {
		return false;
	}
}

void Adjustment_Item::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトがなければ追加する
	datas_[groupName];
}

void Adjustment_Item::SaveFile(const std::string& groupName) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();
	// jsonオブジェクト登録
	root[groupName] = nlohmann::json::object();

	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin();
		itItem != itGroup->second.end(); ++itItem) {
		// 項目名を取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		Item& item = itItem->second;
		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector2>(item)) {
			// Vector2型の値を登録
			Vector2 value = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y});
		}
		else if (std::holds_alternative<Vector3>(item)) {
			// Vector3型の値を登録
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z });
		}
		else if (std::holds_alternative<Vector4>(item)) {
			// Vector4型の値を登録
			Vector4 value = std::get<Vector4>(item);
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z ,value.w });
		}
		std::filesystem::path dir(kDirectoryPath_);
		if (!std::filesystem::exists("Resources/Datas/Adjustment_Item")) {
			std::filesystem::create_directory("Resources/Datas/Adjustment_Item");
		}
		// 書き込むjsonファイルのフルパスを合成する
		std::string filePath = kDirectoryPath_ + groupName + ".json";
		// 書き込み用ファイルストリーム
		std::ofstream ofs;
		// ファイルを書き込みように開く
		ofs.open(filePath);
		//ファイルオープン失敗
		if (ofs.fail()) {
			std::string message = "Failed open data file for write.";
			MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
			assert(0);
			return;
		}
		//ファイルにjson文字列を書き込む(インデント幅4)
		ofs << std::setw(4) << root << std::endl;
		//ファイルを閉じる
		ofs.close();
	}
}

void Adjustment_Item::LoadFiles() {
	if (!std::filesystem::exists(kDirectoryPath_)) {
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
		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void Adjustment_Item::LoadFile(const std::string& groupName) {
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
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();
		if (itItem->is_number_integer()) {
			//int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			//float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_array() && itItem->size() == 2) {
			//float型のjson配列登録
			Vector2 value = { itItem->at(0), itItem->at(1)};
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			//float型のjson配列登録
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 4) {
			//float型のjson配列登録
			Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2),itItem->at(3) };
			SetValue(groupName, itemName, value);
		}
	}
}

void Adjustment_Item::SetValue(
	const std::string& groupName,
	const std::string& key, int32_t value) {
	//グループの参照を取得
	Group& group = datas_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	//設定した項目をstd::mapに追加
	group[key] = newItem;
}

void Adjustment_Item::SetValue(
	const std::string& groupName,
	const std::string& key, float value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void Adjustment_Item::SetValue(const std::string& groupName, const std::string& key, const Vector2& value){
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void Adjustment_Item::SetValue(
	const std::string& groupName,
	const std::string& key, const Vector3& value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void Adjustment_Item::SetValue(const std::string& groupName, const std::string& key, const Vector4& value){
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

int32_t Adjustment_Item::GetIntValue(const std::string& groupName, const std::string& key) {
	// 未登録チェック
	assert(datas_.find(groupName) != datas_.end());
	//グループの参照を取得
	Group& group = datas_[groupName];

	assert(group.find(key) != group.end());

	return std::get<int32_t>(group[key]);

};

float Adjustment_Item::GetfloatValue(const std::string& groupName, const std::string& key) {
	// 未登録チェック
	assert(datas_.find(groupName) != datas_.end());
	// グループの参照を取得
	Group& group = datas_[groupName];

	assert(group.find(key) != group.end());

	return std::get<float>(group[key]);
}

Vector2 Adjustment_Item::GetVector2Value(const std::string& groupName, const std::string& key){
	// 未登録チェック
	assert(datas_.find(groupName) != datas_.end());
	// グループの参照を取得
	Group& group = datas_[groupName];

	assert(group.find(key) != group.end());

	return std::get<Vector2>(group[key]);
}

Vector3 Adjustment_Item::GetVector3Value(const std::string& groupName, const std::string& key) {
	// 未登録チェック
	assert(datas_.find(groupName) != datas_.end());
	// グループの参照を取得
	Group& group = datas_[groupName];

	assert(group.find(key) != group.end());

	return std::get<Vector3>(group[key]);
}

Vector4 Adjustment_Item::GetVector4Value(const std::string& groupName, const std::string& key){
	// 未登録チェック
	assert(datas_.find(groupName) != datas_.end());
	// グループの参照を取得
	Group& group = datas_[groupName];

	assert(group.find(key) != group.end());

	return std::get<Vector4>(group[key]);
}

void Adjustment_Item::AddItem(
	const std::string& groupName, const std::string& key, int32_t value) {
	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {

		// グループ名を取得
		const std::string& groupName_ = itGroup->first;

		// グループの参照を取得
		Group& group = itGroup->second;

		// int型の値を保持していれば
		if (groupName_ == groupName) {
			if ((group.find(key) == group.end())) {
				SetValue(groupName, key, value);
			}
		}
	}

}

void Adjustment_Item::AddItem(
	const std::string& groupName, const std::string& key, float value) {
	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		// グループ名を取得
		const std::string& groupName_ = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;
		// float型の値を保持していれば
		if (groupName_ == groupName) {
			if ((group.find(key) == group.end())) {
				SetValue(groupName, key, value);
			}
		}
	}

}

void Adjustment_Item::AddItem(
	const std::string& groupName, const std::string& key, const Vector2& value) {

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		// グループ名を取得
		const std::string& groupName_ = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;
		// Vector3型の値を保持していれば
		if (groupName_ == groupName) {
			if ((group.find(key) == group.end())) {
				SetValue(groupName, key, value);
			}
		}
	}
}

void Adjustment_Item::AddItem(
	const std::string& groupName, const std::string& key, const Vector3& value) {

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		// グループ名を取得
		const std::string& groupName_ = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;
		// Vector3型の値を保持していれば
		if (groupName_ == groupName) {
			if ((group.find(key) == group.end())) {
				SetValue(groupName, key, value);
			}
		}
	}
}

void Adjustment_Item::AddItem(
	const std::string& groupName, const std::string& key, const Vector4& value){
	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		// グループ名を取得
		const std::string& groupName_ = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;
		// Vector3型の値を保持していれば
		if (groupName_ == groupName) {
			if ((group.find(key) == group.end())) {
				SetValue(groupName, key, value);
			}
		}
	}
}

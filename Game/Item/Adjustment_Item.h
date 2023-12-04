#pragma once
#include<iostream>
#include<variant>
#include<string>
#include<map>
#include"../../externals/imgui/imgui.h"
#include"../../math/Matrix.h"

class Adjustment_Item final {
public:
	static Adjustment_Item* GetInstance();

	//グループの作成
	void CreateGroup(const std::string& groupName);

	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetfloatValue(const std::string& groupName, const std::string& key);
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key);

	// 項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	// 項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	// 項目の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	//ファイルに書き出し
	void SaveFile(const std::string& groupName);

	//ディレクトリの全ファイル読み込み
	void LoadFiles();

	void LoadFile(const std::string& groupName);

	//毎フレーム処理
	void Update();

private:
	//コンストラクタ
	Adjustment_Item() = default;
	//デストラクタ
	~Adjustment_Item() = default;
	//コピーコンストラクタ
	Adjustment_Item(const Adjustment_Item& adjustment_item) = delete;
	//コピー代入演算子
	Adjustment_Item& operator=(const Adjustment_Item&) = delete;

	using Item = std::variant<int32_t, float, Vector3>;

	using Group = std::map<std::string, Item>;

	//全データ
	std::map<std::string, Group> datas_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/Adjustment_Item/";
};


#pragma once
#include"Floor.h"
#include"../../../externals/nlohmann/json.hpp"
#include<list>
#include<fstream>
#include<iostream>
/*床を管理する*/
class FloorManager{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~FloorManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 床追加
	/// </summary>
	void AddFloor(const EulerTransform& transform);

	/// <summary>
	/// imgui表示
	/// </summary>
	void DrawImgui();

public: // アクセッサ

	/// <summary>
	/// 床リストゲッター
	/// </summary>
	/// <returns></returns>
	const std::list<std::unique_ptr<Floor>>& GetFloors()const { return floors_; }

	/// <summary>
	/// デバッグ描画セッター
	/// </summary>
	/// <param name="colliderDebugDraw"></param>
	//void SetColliderDebugDraw(ColliderDebugDraw* colliderDebugDraw) { colliderDebugDraw_ = colliderDebugDraw; }

public:
	using json = nlohmann::json;
	//ファイルに保存
	void SaveFile(const std::vector<std::string>& stages);
	//ファイルを上書き
	void FileOverWrite(const std::string& stage);
	//ファイルを検索
	void ChackFiles();
	//全ファイルを検索
	void LoadFiles(const std::string& stage);
	//ファイルを読み込み
	void LoadFile(const std::string& groupName, const std::string& stage);
	//要素があるか確認
	bool LoadChackItem(const std::string& itemName);

private:

	//json形式からの変換
	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	int chackOnlyNumber_ = 0;

	const std::string kDirectoryPath_ = "Resources/Stages/";

	const std::string kDirectoryName_ = "Resources/Stages";

	const std::string kItemName_ = "Floor";

	std::string Name_ = "\0";

	char ItemName_[256]{};

	std::vector<std::string> fileName_;
private:

	//オブジェクトの情報
	// 床リスト
	std::list<std::unique_ptr<Floor>> floors_;

};


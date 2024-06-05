#pragma once
#include"../../../externals/nlohmann/json.hpp"
#include<list>
#include<fstream>
#include<iostream>
#include"Object3D.h"
#include"SkinAnimObject3D.h"


class LevelLoader{
public:
	LevelLoader() = default;
	~LevelLoader() = default;
	LevelLoader(const LevelLoader& input) = delete;
	LevelLoader& operator=(const LevelLoader&) = delete;

public:
	static LevelLoader* GetInstance();

	/// <summary>
	/// レベルデータを読み込む
	/// </summary>
	void LoadLevelData();

	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 読み込んだレベルデータを取得する
	/// </summary>
	Object3D* GetLevelObject(const std::string tag);

	/// <summary>
	/// 読み込んだレベルデータを取得する
	/// </summary>
	SkinAnimObject3D* GetLevelSkinAnimObject(const std::string tag);

private:
	std::vector < std::pair<std::string, std::unique_ptr<Object3D>>>
		objects_;

	std::vector < std::pair<std::string, std::unique_ptr<SkinAnimObject3D>>>
		skinAnimObjects_;


	/*ファイル制御関連*/
private:
	//メンバ関数
	using json = nlohmann::json;
	//ファイルが存在するか確認する
	void ChackFiles();

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	void from_json(const json& j, Vector2& v);

	//jsonの数値をVector3に変換する関数
	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//選んでいるステージ名
	std::string stageName_;
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "resources/Datas/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "resources/Datas";
	//名前
	inline static const std::string kfileName_ = "SceneData";

	//名前
	inline static const std::string kExtexsion_ = ".json";

	std::vector<std::string> fileName_;
};


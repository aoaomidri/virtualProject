#pragma once
#include"../../../externals/nlohmann/json.hpp"
#include<list>
#include<fstream>
#include<iostream>
#include"Object3D.h"
#include"SkinAnimObject3D.h"
#include <OBB.h>
/*Blenderで生成したjsonを読み込む*/

class LevelLoader{
public:
	LevelLoader() = default;
	~LevelLoader() = default;
	LevelLoader(const LevelLoader& input) = delete;
	LevelLoader& operator=(const LevelLoader&) = delete;

private:
	using json = nlohmann::json;
public:
	static LevelLoader* GetInstance();



	/// <summary>
	/// レベルデータを読み込む
	/// </summary>
	void LoadLevelData();

	/// <summary>
	/// 再帰的に呼び出せるように
	/// </summary>
	void LoadJson(json jData);


	std::vector<Object3D*> GetStationaryObject()const;

	/// <summary>
	/// 読み込んだレベルデータから3Dオブジェを取得する
	/// </summary>
	Object3D* GetLevelObject(const std::string tag);

	/// <summary>
	/// 読み込んだレベルデータからスキニングオブジェを取得する
	/// </summary>
	SkinAnimObject3D* GetLevelSkinAnimObject(const std::string tag);

	/// <summary>
	/// 読み込んだレベルデータから座標を取得する
	/// </summary>
	EulerTransform GetLevelObjectTransform(const std::string tag);

	/// <summary>
	/// 読み込んだレベルデータからOBBコライダーを取得する
	/// </summary>
	OBB GetLevelObjectOBB(const std::string tag);

private:
	std::vector < std::pair<std::string, std::unique_ptr<Object3D>>>
		objects_;

	std::vector < std::pair<std::string, std::unique_ptr<SkinAnimObject3D>>>
		skinAnimObjects_;

	std::vector<std::pair<std::string, OBB>> colliders_;

	std::vector<EulerTransform> boneTransforms_;

	//静止しているオブジェ
	std::vector <std::unique_ptr<Object3D>>
		stationaryObjects_;


	/*ファイル制御関連*/
private:
	
	//ファイルが存在するか確認する
	void ChackFiles();

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

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
	inline static const std::string kfileName_ = "StageData";

	//名前
	inline static const std::string kExtexsion_ = ".json";

	std::vector<std::string> fileName_;
};


#pragma once
#include"Floor.h"
#include"../../../externals/nlohmann/json.hpp"
#include<list>
#include<fstream>
#include<iostream>

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
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(TextureManager* textureManager, const ViewProjection& viewProjection);

	/// <summary>
	/// 床追加
	/// </summary>
	void AddFloor(Transform transform, bool isMoving);

	/// <summary>
	/// imgui表示
	/// </summary>
	void DrawImgui();

public: // アクセッサ

	/// <summary>
	/// 床リストゲッター
	/// </summary>
	/// <returns></returns>
	std::list<Floor*> GetFloors() { return floors_; }

	/// <summary>
	/// デバッグ描画セッター
	/// </summary>
	/// <param name="colliderDebugDraw"></param>
	//void SetColliderDebugDraw(ColliderDebugDraw* colliderDebugDraw) { colliderDebugDraw_ = colliderDebugDraw; }

public:
	using json = nlohmann::json;

	void SaveFile(const std::vector<std::string>& stages);

	void FileOverWrite(const std::string& stage);

	void ChackFiles();

	void LoadFiles(const std::string& stage);

	void LoadFile(const std::string& groupName, const std::string& stage);

	bool LoadChackItem(const std::string& directoryPath, const std::string& itemName);

private:


	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	int chackOnlyNumber = 0;

	const std::string kDirectoryPath = "Resources/Stages/";

	const std::string kDirectoryName = "Resources/Stages";

	const std::string kItemName_ = "Floor";

	std::string Name_ = "\0";

	char ItemName_[256]{};

	std::vector<std::string> fileName;
private:

	//オブジェクトの情報

	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;

	//オブジェクトリスト
	std::list<Object3D*> objects_;

	// 床リスト
	std::list<Floor*> floors_;

};


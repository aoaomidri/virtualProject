#pragma once
#include "Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"ViewProjection.h"
#include"RandomMaker.h"
#include"GraphicsPipeline.h"
#include"TextureManager.h"

class TrailEffect{
public:

	//位置を保存する構造体
	struct PosBuffer
	{
		Vector3 head; //先端の位置
		Vector3 tail; //末端の位置
	};

	//頂点バッファ
	struct SwordTrailVertex
	{
		Vector3 pos = { 0, 0, 0 };
		Vector2 uv = { 0, 0 };
	};
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="bufferSize">vectorコンテナのサイズの設定 </param>
	void Initialize(const int bufferSize);
	//更新処理
	void Update();
	/// <summary>
	/// 現在のフレームの座標を保存する
	/// </summary>
	/// <param name="head">先端</param>
	/// <param name="tail">末端</param>
	void SetPos(const Vector3& head,const Vector3& tail) {
		tempPos_.head = head;
		tempPos_.tail = tail;
	};

	void DrawImgui(std::string name);


	size_t GetBufferSize() {
		return bufferSize_;
	}
private:
	std::vector<PosBuffer>GetUsedPosArray();
	
private:
	//トレイルに関する変数
	//位置を保存するバッファ
	std::vector<PosBuffer> posArray_;
	//頂点バッファ
	std::vector<VertexData> vertex_;
	//現在の座標
	PosBuffer tempPos_;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//利用しているバッファの数
	size_t bufferSize_ = 0;
};


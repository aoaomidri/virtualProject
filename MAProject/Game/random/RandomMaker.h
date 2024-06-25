#pragma once
#include<random>
#include"Matrix.h"

class RandomMaker
{
public:
	static RandomMaker* GetInstance();

	//初期化
	void Initialize();

	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	int DistributionInt(const int min, const int max);

	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	float Distribution(const float min, const float max);

	
	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	Vector2 DistributionV2(const float min, const float max);

	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	Vector3 DistributionV3(const float min, const float max);

private:
	//コンストラクタ
	RandomMaker() = default;
	//デストラクタ
	~RandomMaker() = default;
	//コピーコンストラクタ
	RandomMaker(const RandomMaker& randomMaker) = delete;
	//コピー代入演算子
	RandomMaker& operator=(const RandomMaker&) = delete;


	void MakeRandomEngine();

private:
	std::random_device seedGenerator_;
	std::mt19937 randomEngine_;

};


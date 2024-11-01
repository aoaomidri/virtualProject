#pragma once
#include<random>
#include"Matrix.h"
/*乱数を生成する*/
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
	static int DistributionInt(const int min, const int max);

	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	static float Distribution(const float min, const float max);

	
	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	static Vector2 DistributionV2(const float min, const float max);

	/// <summary>
	/// 最小値と最大値を入れ範囲を決める
	/// </summary>
	/// <param name="min = 最小値"></param>
	/// <param name="max = 最大値"></param>
	/// <returns></returns>
	static Vector3 DistributionV3(const float min, const float max);

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
	static std::random_device seedGenerator_;
	static std::mt19937 randomEngine_;

};


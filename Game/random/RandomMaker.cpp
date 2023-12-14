#include"RandomMaker.h"

RandomMaker* RandomMaker::GetInstance(){
	static RandomMaker instance;

	return &instance;
}

void RandomMaker::Initialize(){
	MakeRandomEngine();
}

float RandomMaker::Distribution(const float min, const float max){
	float result = 0.0f;
	std::uniform_real_distribution<float> distribution(min, max);

	result = distribution(randomEngine_);

	return result;
}

Vector2 RandomMaker::DistributionV2(const float min, const float max) {
	Vector2 result = {};
	std::uniform_real_distribution<float> distribution(min, max);

	result = { distribution(randomEngine_),distribution(randomEngine_) };

	return result;
}

Vector3 RandomMaker::DistributionV3(const float min, const float max) {
	Vector3 result = {};
	std::uniform_real_distribution<float> distribution(min, max);

	result = { distribution(randomEngine_),distribution(randomEngine_),distribution(randomEngine_) };

	return result;
}

void RandomMaker::MakeRandomEngine(){
	std::mt19937 randomEngine(this->seedGenerator_());
	this->randomEngine_ = randomEngine;
}


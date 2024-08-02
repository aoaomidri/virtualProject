#pragma once
#include"ComputeGraphicsPipeline.h"
class ComputePipeLineManager{
public:
	void Initialize();

	//	シングルトンインスタンスの取得
	static ComputePipeLineManager* GetInstance();

	void SetPipeLine();

private:
	std::unique_ptr<ComputeGraphicsPipeline> pipeLine_;
	

};


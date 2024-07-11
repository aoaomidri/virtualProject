#pragma once
#include"TrailEffect.h"
class TrailRender{
public:
	//初期化処理
	void Initialize();

	void Draw();
private:
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailNone_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailNormal_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailAdd_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailSubtract_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailMultily_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrailScreen_;

};


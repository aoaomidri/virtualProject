#include "PostEffect.h"
#include <cassert>
#include"TextureManager.h"
#include"DirectXCommon.h"


void PostEffect::Initialize(){
	//ヴィネッティングなし
	GraphicsPipelineCopy_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineCopy_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/CopyImage.PS.hlsl");
	GraphicsPipelineGray_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGray_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/Grayscale.PS.hlsl");
	GraphicsPipelineSepia_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepia_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/Sepiascale.PS.hlsl");
	GraphicsPipelineInverse_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineInverse_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/InvertedColor.PS.hlsl");
	//ヴィネッティングあり
	GraphicsPipelineNormalVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineNormalVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/NormalVignetting.PS.hlsl");
	GraphicsPipelineGrayVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGrayVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GrayVignetting.PS.hlsl");
	GraphicsPipelineSepiaVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepiaVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/SepiaVignetting.PS.hlsl");

	//順序反転
	GraphicsPipelineVignettingGrayScale_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineVignettingGrayScale_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/VignettingGrayScale.PS.hlsl");
	GraphicsPipelineVignettingSepiaScale_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineVignettingSepiaScale_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/VignettingSepiaScale.PS.hlsl");

	//smoothing
	GraphicsPipelineSmoothing3x3 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing3x3->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter3x3.PS.hlsl");
	GraphicsPipelineSmoothing5x5 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing5x5->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter5x5.PS.hlsl");
	GraphicsPipelineSmoothing9x9 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing9x9->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter9x9.PS.hlsl");

	//outline
	GraphicsPipelineOutLine_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineOutLine_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/DepthBasedOutline.PS.hlsl");

	//ラジアンブラー
	GraphicsPipelineRadialBlur_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineRadialBlur_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/RadialBlur.PS.hlsl");

	CreateVignettingResource();
	ProjectInverseResource();

}

void PostEffect::Finalize(){
	
}

void PostEffect::SetPipeLine(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	if (selectPost_ == 0 || selectPost_ >= PostEffect::Over) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineCopy_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineCopy_->GetPipeLineState());

	}
	else if (selectPost_ == PostEffect::Gray) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineGray_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineGray_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Sepia) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineSepia_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineSepia_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Inverse) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineInverse_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineInverse_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::NormalVignetting) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineNormalVignetting_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineNormalVignetting_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::GrayVignetting) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineGrayVignetting_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineGrayVignetting_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::SepiaVignetting) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineSepiaVignetting_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineSepiaVignetting_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::VignettingGrayScale) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineVignettingGrayScale_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineVignettingGrayScale_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::VignettingSepiaScale) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineVignettingSepiaScale_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineVignettingSepiaScale_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing3x3) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineSmoothing3x3->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineSmoothing3x3->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing5x5) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineSmoothing5x5->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineSmoothing5x5->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing9x9) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineSmoothing9x9->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineSmoothing9x9->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::OutLine) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineOutLine_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineOutLine_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::RadialBlur) {
		commandList->SetGraphicsRootSignature(GraphicsPipelineRadialBlur_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineRadialBlur_->GetPipeLineState());
	}
	else {
		commandList->SetGraphicsRootSignature(GraphicsPipelineCopy_->GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineCopy_->GetPipeLineState());
	}
}

PostEffect* PostEffect::GetInstance(){
	static PostEffect instance;
	return &instance;
}

void PostEffect::CreateVignettingResource(){
	vignettingResource_ = TextureManager::GetInstance()->CreateBufferResource(sizeof(Vignetting));

	vignettingResource_->Map(0, nullptr, reinterpret_cast<void**>(&vignettingData_));

	vignettingData_->scale = 16.0f;

	vignettingData_->pow = 0.8f;
}

void PostEffect::ProjectInverseResource(){
	cameraResource_ = TextureManager::GetInstance()->CreateBufferResource(sizeof(CameraMat));

	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->matProjectionInverse_.Identity();
}

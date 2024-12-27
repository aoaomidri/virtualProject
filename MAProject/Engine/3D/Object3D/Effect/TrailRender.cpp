#include "TrailRender.h"
#include"DirectXCommon.h"

void TrailRender::Initialize(){
	GraphicsPipelineTrail_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineTrail_->InitializeWorld2D(L"resources/shaders/object2D/Object2d.VS.hlsl", L"resources/shaders/object2D/Object2d.PS.hlsl");
	selectTrail_ = EffectType::Glound;
	MakeResource();
}

void TrailRender::Draw(const TrailEffect* trail, const Matrix4x4& viewPro){
	if (trail == nullptr || trail->GetVertexSize() < 5) {
		return; // trailがnullptrまたは頂点サイズが不足している場合
	}
	if (!isDraw_) {
		return; // 描画しないと宣言しているとき
	}

	switch (selectTrail_) {
	case EffectType::Water:
		trailColor_.SetVector4(waterColor_);
		break;
	case EffectType::Glound:
		trailColor_.SetVector4(groundColor_);
		break;
	default:
		trailColor_.SetVector4({ 1.0f,1.0f,1.0f,1.0f });
		break;

	}
	materialDate_->color = trailColor_;
	*wvpData_ = viewPro;


	auto command = DirectXCommon::GetInstance()->GetCommandList();

	command->SetGraphicsRootSignature(GraphicsPipelineTrail_->GetRootSignature());
	command->SetPipelineState(GraphicsPipelineTrail_->GetPipeLineState());

	//形状を設定。
	command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	trail->SetBuffers();
	command->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	command->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	command->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(trail->GetTextureHandle()));

	command->DrawIndexedInstanced(static_cast<uint32_t>(trail->GetIndexSize()), 1, 0, 0, 0);
}


void TrailRender::MakeResource(){
	auto* textureManager = TextureManager::GetInstance();

	//マテリアル用のリソース
	materialResource_ = textureManager->CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = false;

	materialDate_->uvTransform.Identity();

	materialDate_->shininess = 1.0f;

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpResource_ = textureManager->CreateBufferResource(sizeof(Matrix4x4));
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	*wvpData_ = Matrix::MakeIdentity4x4();

}

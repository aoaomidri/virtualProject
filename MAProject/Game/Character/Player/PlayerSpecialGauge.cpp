#include "PlayerSpecialGauge.h"

void PlayerSpecialGauge::ApplyGlobalVariables(){
	//ファイルから読み込んで値を代入する
	gaugeMax_ = adjustment_item_->GetfloatValue(groupName_, "gaugeMax");
	hitAddGauge_ = adjustment_item_->GetfloatValue(groupName_, "hitAddGauge");
	knockDownAddGauge_ = adjustment_item_->GetfloatValue(groupName_, "knockDownAddGauge");
	specialAttackConsumption_ = adjustment_item_->GetfloatValue(groupName_, "specialAttackConsumption");

	gaugeFrameTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "gaugeFrameTexPos");
	gaugeFrameTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "gaugeFrameTexScale");
	gaugeTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "gaugeTexPos");
	gaugeTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "gaugeTexScale");
}

void PlayerSpecialGauge::ExportGlobalVariables(){
	adjustment_item_ = Adjustment_Item::GetInstance();
	//グループを追加
	adjustment_item_->CreateGroup(groupName_);
	//アイテムの追加
	adjustment_item_->AddItem(groupName_, "gaugeMax", gaugeMax_);
	adjustment_item_->AddItem(groupName_, "hitAddGauge", hitAddGauge_);
	adjustment_item_->AddItem(groupName_, "knockDownAddGauge", knockDownAddGauge_);
	adjustment_item_->AddItem(groupName_, "specialAttackConsumption", specialAttackConsumption_);
	//テクスチャ関連
	adjustment_item_->AddItem(groupName_, "gaugeFrameTexPos", gaugeFrameTex_->position_);
	adjustment_item_->AddItem(groupName_, "gaugeFrameTexScale", gaugeFrameTex_->scale_);

	adjustment_item_->AddItem(groupName_, "gaugeTexPos", gaugeTex_->position_);
	adjustment_item_->AddItem(groupName_, "gaugeTexScale", gaugeTex_->scale_);
}

void PlayerSpecialGauge::TextureInitialize(){
	uint32_t textureHandle = 0;

	textureManager_ = TextureManager::GetInstance();

	gaugeFrameTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/PlayerStatesTex/Gauge.png");
	gaugeFrameTex_->Initialize(textureHandle);

	gaugeTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Whitex64.png");
	gaugeTex_->Initialize(textureHandle);
}

void PlayerSpecialGauge::Initialize(){
	TextureInitialize();
	ExportGlobalVariables();

}


void PlayerSpecialGauge::Update(){
	ApplyGlobalVariables();
	DrawImgui();

	

}

void PlayerSpecialGauge::Draw(){
	gaugeFrameTex_->Draw();
	gaugeTex_->Draw();
}

void PlayerSpecialGauge::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("ゲージ関連");
	ImGui::DragFloat2("必殺ゲージ(中身)の座標", &gaugeTex_->position_.x, 1.0f);
	ImGui::DragFloat2("必殺ゲージ(中身)の長さ", &gaugeTex_->textureAddRightPos_.x, 1.0f);
	ImGui::Text("ゲージの色");
	ImGui::ColorEdit4("不透明の色", &opaqueColor_.x);
	ImGui::ColorEdit4("半透明の色", &translucentColor_.x);
	ImGui::End();

	
#endif
}

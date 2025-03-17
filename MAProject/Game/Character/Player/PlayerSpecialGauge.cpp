#include "PlayerSpecialGauge.h"

void PlayerSpecialGauge::ApplyGlobalVariables(){
	//ファイルから読み込んで値を代入する
	gaugeMax_ = adjustment_item_->GetfloatValue(groupName_, "gaugeMax");
	hitAddGauge_ = adjustment_item_->GetfloatValue(groupName_, "hitAddGauge");
	knockDownAddGauge_ = adjustment_item_->GetfloatValue(groupName_, "knockDownAddGauge");
	specialAttackConsumption_ = adjustment_item_->GetfloatValue(groupName_, "specialAttackConsumption");
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
}

void PlayerSpecialGauge::Initialize(){
	ExportGlobalVariables();

}

void PlayerSpecialGauge::Update(){
	ApplyGlobalVariables();

	DrawImgui();
}

void PlayerSpecialGauge::Draw(){

}

void PlayerSpecialGauge::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("ゲージの色味");
	ImGui::ColorEdit4("不透明の色", &opaqueColor_.x);
	ImGui::ColorEdit4("半透明の色", &translucentColor_.x);
	ImGui::End();
#endif
}

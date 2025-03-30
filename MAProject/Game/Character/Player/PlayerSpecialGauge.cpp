#include "PlayerSpecialGauge.h"
#include"Ease/Ease.h"
#include"GameTime.h"

void PlayerSpecialGauge::ApplyGlobalVariables(){
	//ファイルから読み込んで値を代入する
	gaugeMax_ = adjustment_item_->GetfloatValue(groupName_, "gaugeMax");
	hitAddGauge_ = adjustment_item_->GetfloatValue(groupName_, "hitAddGauge");

	gaugeFrameTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "gaugeFrameTexPos");
	gaugeFrameTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "gaugeFrameTexScale");
	gaugeTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "gaugeTexPos");
	gaugeTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "gaugeTexScale");
	buttonTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "buttonTexPos");
	buttonTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "buttonTexScale");
}

void PlayerSpecialGauge::ExportGlobalVariables(){
	adjustment_item_ = Adjustment_Item::GetInstance();
	//グループを追加
	adjustment_item_->CreateGroup(groupName_);
	//アイテムの追加
	adjustment_item_->AddItem(groupName_, "gaugeMax", gaugeMax_);
	adjustment_item_->AddItem(groupName_, "hitAddGauge", hitAddGauge_);
	//テクスチャ関連
	adjustment_item_->AddItem(groupName_, "gaugeFrameTexPos", gaugeFrameTex_->position_);
	adjustment_item_->AddItem(groupName_, "gaugeFrameTexScale", gaugeFrameTex_->scale_);
	adjustment_item_->AddItem(groupName_, "gaugeTexPos", gaugeTex_->position_);
	adjustment_item_->AddItem(groupName_, "gaugeTexScale", gaugeTex_->scale_);
	adjustment_item_->AddItem(groupName_, "buttonTexPos", buttonTex_->position_);
	adjustment_item_->AddItem(groupName_, "buttonTexScale", buttonTex_->scale_);
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
	gaugeTex_->textureAddRightPos_.x = kGaugeMinMax_.x;

	buttonTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/BButton.png");
	buttonTex_->Initialize(textureHandle);
	
}

void PlayerSpecialGauge::Initialize(){
	TextureInitialize();
	ExportGlobalVariables();

	gaugelengthMax_ = -kGaugeMinMax_.x + kGaugeMinMax_.y;	
}


void PlayerSpecialGauge::Update(){
	ApplyGlobalVariables();
	DrawImgui();
	GaugeUpdate();
}

void PlayerSpecialGauge::Draw(){
	gaugeTex_->Draw();
	gaugeFrameTex_->Draw();
	buttonTex_->Draw();
}

void PlayerSpecialGauge::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("ゲージ関連");
	ImGui::DragFloat2("必殺ゲージ(中身)の座標", &gaugeTex_->position_.x, 1.0f);
	ImGui::DragFloat2("必殺ゲージ(中身)の長さ", &gaugeTex_->textureAddRightPos_.x, 1.0f);
	ImGui::DragFloat("現在のゲージ割合", &nowGauge_, 0.01f, 0.0f, 10.0f);
	ImGui::Text("ゲージの色");
	ImGui::Checkbox("使えるかどうかのフラグ", &isUse_);
	ImGui::ColorEdit4("不透明の色", &opaqueColor_.x);
	ImGui::ColorEdit4("半透明の色", &translucentColor_.x);
	ImGui::End();	
#endif
}

void PlayerSpecialGauge::GaugeUpdate(){
	//必殺発動条件は最大値の半分以上
	specialAttackConsumption_ = (gaugeMax_ / 2.0f);

	//最大値が決まっているかつ、ゲージの最大が1でなければゲージの最大値によって補正を掛ける
	if (gaugeMax_ != 0.0f and gaugeMax_ != 1.0f){
		//ゲージが最大を超えたら補正
		if (nowGauge_ >= gaugeMax_) {
			nowGauge_ = gaugeMax_;
		}
		float newNowGauge = (nowGauge_ / gaugeMax_);
		//ゲージの割合から長さを求める
		nowGaugeLength_ = Ease::Easing(Ease::EaseName::Lerp, kGaugeMinMax_.x, kGaugeMinMax_.y, newNowGauge);
		//現在のゲージが発動条件を超えているか
		if (nowGauge_ >= specialAttackConsumption_){
			isUse_ = true;
		}
		else {
			isUse_ = false;
		}
	}
	else {
		//ゲージが1を超えたら補正
		if (nowGauge_ >= 1.0f) {
			nowGauge_ = 1.0f;
		}
		//ゲージの割合から長さを求める
		nowGaugeLength_ = Ease::Easing(Ease::EaseName::Lerp, kGaugeMinMax_.x, kGaugeMinMax_.y, nowGauge_);
		//現在のゲージが発動条件を超えているか
		if (nowGauge_ >= specialAttackConsumption_) {
			isUse_ = true;
		}
		else {
			isUse_ = false;
		}
	}

	gaugeTex_->textureAddRightPos_.x = nowGaugeLength_;

	//発動ができるかどうかで色を変更
	if (isUse_) {
		gaugeTex_->color_ = opaqueColor_;
		if (GameTime::isDecimalAboveFive()){
			buttonTex_->color_.w = buttonAlpha_;
		}
		else {
			buttonTex_->color_.w = 0.0f;
		}
		
	}
	else {
		gaugeTex_->color_ = translucentColor_;
		buttonTex_->color_.w = 0.0f;
	}


}

#include "GameUIManager.h"

void GameUIManager::ApplyGlobalVariables(){
	weakComboTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "weakTexPos");
	weakComboTex_->scale_=adjustment_item_->GetVector2Value(groupName_, "weakTexScale");
	checkMarkTex_[0]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark1Pos");
	checkMarkTex_[1]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark2Pos");
	checkMarkTex_[2]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark3Pos");
	checkMarkTex_[3]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark4Pos");
	checkMarkTex_[4]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark5Pos");
	checkMarkTex_[5]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark6Pos");
	checkScale_ = adjustment_item_->GetfloatValue(groupName_, "checkMarkScale");
}

void GameUIManager::ExportGlobalVariables(){
	adjustment_item_ = Adjustment_Item::GetInstance();
	//グループを追加
	adjustment_item_->CreateGroup(groupName_);
	//アイテムの追加
	adjustment_item_->AddItem(groupName_, "weakTexPos", weakComboTex_->position_);
	adjustment_item_->AddItem(groupName_, "weakTexScale", weakComboTex_->scale_);
	adjustment_item_->AddItem(groupName_, "checkMark1Pos", checkMarkTex_[0]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark2Pos", checkMarkTex_[1]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark3Pos", checkMarkTex_[2]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark4Pos", checkMarkTex_[3]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark5Pos", checkMarkTex_[4]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark6Pos", checkMarkTex_[5]->position_);
	adjustment_item_->AddItem(groupName_, "checkMarkScale", checkScale_);
}

void GameUIManager::Initialize(){
	plStateManager_ = PlayerStateManager::GetInstance();

	//テクスチャの初期化
	TextureInitialize();
	ExportGlobalVariables();

	actionTextSprite_->position_ = { 1117.0f,531.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->scale_ = { 242.0f,137.0f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1072.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,0.0f };

	weakComboTex_->anchorPoint_ = { 0.5f,0.5f };
	weakComboTex_->color_ = { 1.0f,1.0f,1.0f,1.0f };
}

void GameUIManager::TextureInitialize(){
	uint32_t textureHandle = 0;

	textureManager_ = TextureManager::GetInstance();

	actionTextSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/actionText.png");
	actionTextSprite_->Initialize(textureHandle);

	attackSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/STAttack.png");
	attackSprite_->Initialize(textureHandle);

	weakComboTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/controlWeak.png");
	weakComboTex_->Initialize(textureHandle);

	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i] = std::make_unique<Sprite>();
		textureHandle = textureManager_->Load("resources/texture/Mark/checkMark.png");
		checkMarkTex_[i]->Initialize(textureHandle);
	}
}

void GameUIManager::Update(){
	ApplyGlobalVariables();

	TutorialUpdate();

	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i]->scale_ = { checkScale_,checkScale_ };
	}

#ifdef _DEBUG
		ImGui::Begin("テクスチャの座標など");
		ImGui::DragFloat2("座標", &weakComboTex_->position_.x, 1.0f);
		ImGui::DragFloat2("大きさ", &weakComboTex_->scale_.x, 1.0f);

		ImGui::DragFloat2("X攻撃の座標", &actionTextSprite_->position_.x, 1.0f);
		ImGui::DragFloat2("X攻撃の大きさ", &actionTextSprite_->scale_.x, 1.0f);
		ImGui::End();
#endif
}

void GameUIManager::TutorialUpdate(){
	PlayerStateManager::StateName nowStateName = plStateManager_->GetStateName();
	//攻撃状態であれば描画
	if (nowStateName == PlayerStateManager::StateName::Attack or nowStateName == PlayerStateManager::StateName::StrongAttack){
		//強攻撃時はコンボを参照しない
		if (nowStateName != PlayerStateManager::StateName::StrongAttack) {
			int combo = plStateManager_->GetComboIndex();
			for (size_t i = 0; i < combo; i++) {
				checkMarkTex_[i]->color_.w = 1.0f;
			}
		}
	}
	else {
		for (size_t i = 0; i < comboMax_; i++) {
			checkMarkTex_[i]->color_.w = 0.0f;
		}
	}

	
}

void GameUIManager::Draw(){

	weakComboTex_->Draw();
	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i]->Draw();
	}

	actionTextSprite_->Draw();
	attackSprite_->Draw();
}

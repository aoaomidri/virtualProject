#include "GameUIManager.h"

void GameUIManager::ApplyGlobalVariables(){
	weakComboTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "weakTexPos");
	weakComboTex_->scale_=adjustment_item_->GetVector2Value(groupName_, "weakTexScale");
	strong2ComboTex_->position_ = adjustment_item_->GetVector3Value(groupName_, "strong2TexPos");
	strong2ComboTex_->scale_ = adjustment_item_->GetVector2Value(groupName_, "strong2TexScale");
	attackSprite_->position_ = adjustment_item_->GetVector3Value(groupName_, "attackSTTexPos");
	attackSprite_->scale_ = adjustment_item_->GetVector2Value(groupName_, "attackSTTexScale");
	checkMarkTex_[0]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark1Pos");
	checkMarkTex_[1]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark2Pos");
	checkMarkTex_[2]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark3Pos");
	checkMarkTex_[3]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark4Pos");
	checkMarkTex_[4]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark5Pos");
	checkMarkTex_[5]->position_ = adjustment_item_->GetVector3Value(groupName_, "checkMark6Pos");
	checkPosStrong_[0] = adjustment_item_->GetVector3Value(groupName_, "checkMark1PosStrong");
	checkPosStrong_[1] = adjustment_item_->GetVector3Value(groupName_, "checkMark2PosStrong");
	checkPosStrong_[2] = adjustment_item_->GetVector3Value(groupName_, "checkMark3PosStrong");
	checkPosStrong_[3] = adjustment_item_->GetVector3Value(groupName_, "checkMark4PosStrong");
	checkPosStrong_[4] = adjustment_item_->GetVector3Value(groupName_, "checkMark5PosStrong");
	checkPosStrong_[5] = adjustment_item_->GetVector3Value(groupName_, "checkMark6PosStrong");
	checkScale_ = adjustment_item_->GetfloatValue(groupName_, "checkMarkScale");
}

void GameUIManager::ExportGlobalVariables(){
	adjustment_item_ = Adjustment_Item::GetInstance();
	//グループを追加
	adjustment_item_->CreateGroup(groupName_);
	//アイテムの追加
	adjustment_item_->AddItem(groupName_, "weakTexPos", weakComboTex_->position_);
	adjustment_item_->AddItem(groupName_, "weakTexScale", weakComboTex_->scale_);

	adjustment_item_->AddItem(groupName_, "strong2TexPos", strong2ComboTex_->position_);
	adjustment_item_->AddItem(groupName_, "strong2TexScale", strong2ComboTex_->scale_);

	adjustment_item_->AddItem(groupName_, "attackSTTexPos", attackSprite_->position_);
	adjustment_item_->AddItem(groupName_, "attackSTTexScale", attackSprite_->scale_);

	adjustment_item_->AddItem(groupName_, "checkMark1Pos", checkMarkTex_[0]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark2Pos", checkMarkTex_[1]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark3Pos", checkMarkTex_[2]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark4Pos", checkMarkTex_[3]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark5Pos", checkMarkTex_[4]->position_);
	adjustment_item_->AddItem(groupName_, "checkMark6Pos", checkMarkTex_[5]->position_);

	adjustment_item_->AddItem(groupName_, "checkMark1PosStrong", checkPosStrong_[0]);
	adjustment_item_->AddItem(groupName_, "checkMark2PosStrong", checkPosStrong_[1]);
	adjustment_item_->AddItem(groupName_, "checkMark3PosStrong", checkPosStrong_[2]);
	adjustment_item_->AddItem(groupName_, "checkMark4PosStrong", checkPosStrong_[3]);
	adjustment_item_->AddItem(groupName_, "checkMark5PosStrong", checkPosStrong_[4]);
	adjustment_item_->AddItem(groupName_, "checkMark6PosStrong", checkPosStrong_[5]);
	adjustment_item_->AddItem(groupName_, "checkMarkScale", checkScale_);
}

void GameUIManager::Initialize(){
	plStateManager_ = PlayerStateManager::GetInstance();
	titorialLevel_ = 1;
	//テクスチャの初期化
	TextureInitialize();
	ExportGlobalVariables();

	spGauge_ = std::make_unique<PlayerSpecialGauge>();
	spGauge_->Initialize();

	actionTextSprite_->position_ = { 1117.0f,531.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->scale_ = { 242.0f,137.0f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1122.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	weakComboTex_->anchorPoint_ = { 0.5f,0.5f };
	weakComboTex_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	strong2ComboTex_->anchorPoint_ = { 0.5f,0.5f };
	strong2ComboTex_->color_ = { 1.0f,1.0f,1.0f,1.0f };
}

void GameUIManager::TextureInitialize(){
	uint32_t textureHandle = 0;

	textureManager_ = TextureManager::GetInstance();

	actionTextSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Text/actionText.png");
	actionTextSprite_->Initialize(textureHandle);

	attackSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Text/STAttack.png");
	attackSprite_->Initialize(textureHandle);

	weakComboTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/combo/controlWeak.png");
	weakComboTex_->Initialize(textureHandle);

	strong2ComboTex_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/combo/Strong2AttackTex.png");
	strong2ComboTex_->Initialize(textureHandle);

	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i] = std::make_unique<Sprite>();
		textureHandle = textureManager_->Load("resources/texture/Mark/checkMark.png");
		checkMarkTex_[i]->Initialize(textureHandle);
	}
}

void GameUIManager::Update(){
	ApplyGlobalVariables();
	spGauge_->Update();

	TutorialUpdate();

	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i]->scale_ = { checkScale_,checkScale_ };
	}
}

void GameUIManager::TutorialUpdate(){
	PlayerStateManager::StateName nowStateName = plStateManager_->GetStateName();
	if (titorialLevel_ == 1){
		Tutorial1Update(nowStateName);
	}
	else if (titorialLevel_ == 2){
		Tutorial2Update(nowStateName);
	}
	if (CheckWeak_ and CheckStrong_){
		isTutorial_ = false;
	}
}

void GameUIManager::Tutorial1Update(const PlayerStateManager::StateName state){
	//攻撃状態であれば描画
	if (state == PlayerStateManager::StateName::Attack or state == PlayerStateManager::StateName::StrongAttack) {
		//強攻撃時はコンボを参照しない
		if (state != PlayerStateManager::StateName::StrongAttack) {
			int combo = plStateManager_->GetComboIndex();
			for (size_t i = 0; i < combo; i++) {
				checkMarkTex_[i]->color_.w = 1.0f;
			}
			if (checkMarkTex_[5]->color_.w == 1.0) {
				titorialLevel_++;
				for (size_t i = 0; i < comboMax_; i++) {
					checkMarkTex_[i]->color_.w = 0.0f;
				}
			}
		}
	}
	else {
		for (size_t i = 0; i < comboMax_; i++) {
			checkMarkTex_[i]->color_.w = 0.0f;
		}
	}
}

void GameUIManager::Tutorial2Update(const PlayerStateManager::StateName state){
	for (size_t i = 0; i < comboMax_; i++) {
		checkMarkTex_[i]->position_ = checkPosStrong_[i];
	}
	//攻撃状態であれば描画
	if (state == PlayerStateManager::StateName::Attack or state == PlayerStateManager::StateName::StrongAttack) {
		int combo = plStateManager_->GetComboIndex();
		if (combo > 1) {
			//弱攻撃連打であれば早期リターン
			if (state == PlayerStateManager::StateName::Attack) {
				return;
			}
		}
		if (combo == 2) {
			if (Input::GetInstance()->GetPadButtonTriger(Input::GamePad::X)) {
				checkMarkTex_[2]->color_.w = 1.0f;
				CheckWeak_ = true;
			}
			else if (Input::GetInstance()->GetPadButtonTriger(Input::GamePad::Y)) {
				checkMarkTex_[3]->color_.w = 1.0f;
				CheckStrong_ = true;
			}
		}

		for (size_t i = 0; i < combo; i++) {
			checkMarkTex_[i]->color_.w = 1.0f;
		}
	}
	else {
		for (size_t i = 0; i < comboMax_; i++) {
			checkMarkTex_[i]->color_.w = 0.0f;
		}
	}
}

void GameUIManager::Draw(){
	
	if (isTutorial_) {
		if (titorialLevel_ == 1) {
			weakComboTex_->Draw();
		}
		else {
			strong2ComboTex_->Draw();
		}

		for (size_t i = 0; i < comboMax_; i++) {
			checkMarkTex_[i]->Draw();
		}
	}
	spGauge_->Draw();
	actionTextSprite_->Draw();
	attackSprite_->Draw();
}

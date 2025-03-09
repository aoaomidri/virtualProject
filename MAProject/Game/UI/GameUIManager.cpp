#include "GameUIManager.h"

void GameUIManager::Initialize(){
	//テクスチャの初期化
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

	actionTextSprite_->position_ = { 1117.0f,531.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->scale_ = { 242.0f,137.0f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1072.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,0.0f };

	weakComboTex_->position_ = { 649.0f,633.0f };
	weakComboTex_->scale_ = { 524.0f,71.0f };
	weakComboTex_->anchorPoint_ = { 0.5f,0.5f };
	weakComboTex_->color_ = { 1.0f,1.0f,1.0f,1.0f };
}

void GameUIManager::Update(){
#ifdef _DEBUG
		ImGui::Begin("テクスチャの座標など");
		ImGui::DragFloat2("座標", &weakComboTex_->position_.x, 1.0f);
		ImGui::DragFloat2("大きさ", &weakComboTex_->scale_.x, 1.0f);

		ImGui::DragFloat2("X攻撃の座標", &actionTextSprite_->position_.x, 1.0f);
		ImGui::DragFloat2("X攻撃の大きさ", &actionTextSprite_->scale_.x, 1.0f);
		ImGui::End();
#endif
}

void GameUIManager::Draw(){
	weakComboTex_->Draw();

	actionTextSprite_->Draw();
	attackSprite_->Draw();
}

#pragma once
#include"BasePlayerState.h"
#include"PlayerRoot.h"
#include"PlayerDash.h"
#include"PlayerAttack.h"
#include"PlayerStrongAttack.h"
#include<memory>

class PlayerStateManager{
public:
	using StateName = BasePlayerState::StateName;
	using PLContext = BasePlayerState::PlayerContext;

	PlayerStateManager() = default;
	~PlayerStateManager() = default;
	PlayerStateManager(const PlayerStateManager& stateManager) = delete;
	PlayerStateManager& operator=(const PlayerStateManager&) = delete;

	static PlayerStateManager* GetInstance() {
		static PlayerStateManager instance;
		return &instance;
	}
	//ファイルから読み込み代入する
	void ApplyGlobalVariables();
	//変数をImguiやファイルに書き出す
	void InitGlobalVariables() const;

	//プレイヤーのステータスを初期化する
	void InitState();

	//状態の変更
	void ChangeState(const StateName nextState);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate);

	/// <summary>
	/// 共通の値の更新処理
	/// </summary>
	void ContextStateUpdate();

public:
	//Getter
	const StateName& GetStateName()const { return nowStateName_; }
	
	const EulerTransform& GetPlayerTrnaform()const { return context_.playerTransform_; }
	const EulerTransform& GetWeaponTrnaform()const { return context_.weaponParameter_.weaponTransform_; }
	const EulerTransform& GetWeaponCollisionTrnaform()const { return context_.weaponParameter_.weaponCollisionTransform_; }

	const Matrix4x4& GetPlayerRotateMatrix()const { return context_.playerRotateMatrix_; }

	const int GetComboIndex()const { return context_.workAttack_.comboIndex_; }

	const float GetGroundCrushTexAlpha()const { return context_.groundCrushTexAlpha_; }

	const bool GetIsDash()const { return context_.workDash_.isDash_; }
	const bool GetTrailResetFlug()const { return context_.workAttack_.trailResetFlug_; }
	const bool GetHitRecordResetFlug()const { return context_.workAttack_.hitRecordRestFlug_; }
	const bool GetIsDissolve()const { return context_.isDissolve_; }
	const bool GetIsStopCrush()const { return context_.isStopCrush_; }
	

	//Setter
	void SetPlayerTranslateY(const float posY) { context_.playerTransform_.translate.y = posY; }

	void SetTrailResetFlug(const bool flug) { context_.workAttack_.trailResetFlug_ = flug; }
	void SetHitRecordResetFlug(const bool flug) { context_.workAttack_.hitRecordRestFlug_ = flug; }
	void SetIsCollisionEnemy(const bool flug){context_.isCollisionEnemy_ = flug; }
	void SetIsDissolve(const bool flug) { context_.isDissolve_ = flug; }
	void SetIsStopCrush(const bool flug) { context_.isStopCrush_ = flug; }

private:
	//今の状態の名前
	StateName nowStateName_{};

	//今の状態
	std::unique_ptr<BasePlayerState> nowState_;
	//次の状態
	std::unique_ptr<BasePlayerState> nextState_;

	PLContext context_;

	//ヒビの透明化速度
	float crushColorMinus_ = 0.02f;

};


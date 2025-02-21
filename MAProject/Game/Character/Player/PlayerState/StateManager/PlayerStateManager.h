#pragma once
#include"BasePlayerState.h"
#include"PlayerRoot.h"
#include"PlayerDash.h"
#include"PlayerAttack.h"
#include"PlayerStrongAttack.h"
#include"PlayerJustAvoid.h"
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

	void JumpFlugReset();

public:
	//Getter
	const StateName& GetStateName()const { return nowStateName_; }

	const HitRecord::KnockbackType GetKnockbackType()const { return context_.workAttack_.type_; }
	
	const EulerTransform& GetPlayerTrnaform()const { return context_.playerTransform_; }
	const EulerTransform& GetPlayerAppearanceTrnaform()const { return context_.appearanceTransform_; }
	const EulerTransform& GetWeaponTrnaform()const { return context_.weaponParameter_.weaponTransform_; }
	const EulerTransform& GetWeaponCollisionTrnaform()const { return context_.weaponParameter_.weaponCollisionTransform_; }

	const Matrix4x4& GetPlayerRotateMatrix()const { return context_.playerRotateMatrix_; }

	const int GetComboIndex()const { return context_.workAttack_.comboIndex_; }
	const int GetJustAvoidTimer()const { return context_.workAvoidAttack_.justAvoidAttackTimer_; }

	const float GetGroundCrushTexAlpha()const { return context_.groundCrushTexAlpha_; }
	const float GetJumpPower()const { return context_.jumpPower_; }

	const bool GetIsDash()const { return context_.workDash_.isDash_; }
	const bool GetIsJump()const { return context_.isJump_; }
	const bool GetIsAttackJump()const { return context_.isAttackJump_; }
	const bool GetTrailResetFlug()const { return context_.workAttack_.trailResetFlug_; }
	const bool GetHitRecordResetFlug()const { return context_.workAttack_.hitRecordRestFlug_; }
	const bool GetIsDissolve()const { return context_.isDissolve_; }
	const bool GetIsStopCrush()const { return context_.isStopCrush_; }
	const bool GetIsGuard()const { return context_.isGuard_; }
	const bool GetIsGuardHit()const { return context_.isGuardHit_; }
	const bool GetIsDrawTrail()const { return context_.isTrail_; }
	const bool GetIsHitStopFlug()const { return context_.workAttack_.isStrongHitStop_; }
	//Setter
	void SetJustAvoidTimer() { context_.workAvoidAttack_.justAvoidAttackTimer_ = context_.workAvoidAttack_.justAvoidAttackTimerBase_; }

	void SetPlayerTranslateY(const float posY) { context_.playerTransform_.translate.y = posY; }

	void SetTrailResetFlug(const bool flug) { context_.workAttack_.trailResetFlug_ = flug; }
	void SetHitRecordResetFlug(const bool flug) { context_.workAttack_.hitRecordRestFlug_ = flug; }
	void SetIsCollisionEnemy(const bool flug){context_.isCollisionEnemy_ = flug; }
	void SetIsJustAvoid(const bool flug) { context_.isJustAvoid_ = flug; }
	void SetIsDissolve(const bool flug) { context_.isDissolve_ = flug; }
	void SetIsStopCrush(const bool flug) { context_.isStopCrush_ = flug; }
	void SetIsGuardHit(const bool flug) { context_.isGuardHit_ = flug; }
	void SetIsOnFloor(const bool flug) { context_.isOnFloor_ = flug; }

	void SetLockOnPos(const Vector3& pos) { context_.lockOnPos_ = pos; }

private:
	void ResetState();
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


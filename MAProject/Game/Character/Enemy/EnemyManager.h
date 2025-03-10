#pragma once
#include"Enemy.h"
#include"BossEnemy.h"
#include<list>
#include"FollowCamera.h"

/*敵の統括するクラス*/

class EnemyManager{
public:
	EnemyManager();

	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画
	void Draw(const FollowCamera* camera);
	//テクスチャ描画
	void TexDraw(const FollowCamera* camera);
	//パーティクル描画
	void ParticleDraw(const FollowCamera* camera, const Vector3& color);

	//Imgui描画
	void DrawImgui();
	//敵の数を増やす
	void AddEnemys();

public:
	//ゲッター
	std::list<std::unique_ptr<Enemy>>& GetEnemies() {
		if (isTutoria_){
			return tutorialEnemies_;
		}
		else {
			return enemies_;
		}
	}

	BossEnemy* GetBossEnemy() { return boss_.get(); }

	const bool GetIsTutorial() const { return isTutoria_; }

	const size_t GetEnemyNum()const {
		if (isTutoria_) {
			return tutorialEnemies_.size();
		}
		else {
			return enemies_.size();
		}
	}

	//セッター
	void SetTarget(const EulerTransform* target) { targetTrans_ = target; }

	void SetTargetMat(const Matrix4x4* target) { targetRotateMat_ = target; }

	void SetShininess(const float shininess) {
		enemyShininess_ = shininess;
	}

	void SetTimeScale(const float scale) { timeScale_ = scale; }

	//セッター
	void SetTutorialFlug(const bool flug) { isTutoria_ = flug; }
private:
	float timeScale_ = 0.0f;
	//敵の反射強度
	float enemyShininess_ = 0.0f;

	//リストに詰める用
	std::unique_ptr<Enemy> enemy_;

	std::unique_ptr<BossEnemy> boss_;
	//敵のリスト
	std::list<std::unique_ptr<Enemy>> enemies_;	
	//チュートリアル用敵のリスト
	std::list<std::unique_ptr<Enemy>> tutorialEnemies_;

	std::unique_ptr<EnemyAttackTicket>tickets_;
	//敵の初期最大値
	static const int32_t enemyNum_ = 80;
	//チュートリアル用敵の初期最大値
	static const int32_t tutorialEnemyNum_ = 30;

	//チュートリアル用のフラグ
	bool isTutoria_ = true;

	//追加する敵の数
	const int32_t addEnemyNum_ = 1;
	//ひとまず手打ちでの敵の初期値座標
	Vector3 bossPos_ = { 0.0f,2.5f * 1.6f,20.0f };
	std::array<Vector3, enemyNum_> enemysPos_{};
	//プレイヤーの座標
	const EulerTransform* targetTrans_ = nullptr;
	//プレイヤーの回転
	const Matrix4x4* targetRotateMat_ = nullptr;	
};


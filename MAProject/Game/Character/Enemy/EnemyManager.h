#pragma once
#include"Enemy.h"
#include<list>

#include"FollowCamera.h"

/*敵の統括するクラス*/

class EnemyManager{
public:
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
	std::list<std::unique_ptr<Enemy>> &GetEnemies() { return enemies_; }

	const size_t GetEnemyNum()const { return enemies_.size(); }

	//セッター
	void SetTarget(const EulerTransform* target) { targetTrans_ = target; }

	void SetTargetMat(const Matrix4x4* target) { targetRotateMat_ = target; }

	void SetShininess(const float shininess) {
		enemyShininess_ = shininess;
	}

	void SetTimeScale(const float scale) { timeScale_ = scale; }


private:
	float timeScale_ = 0.0f;
	//敵の反射強度
	float enemyShininess_ = 0.0f;

	//リストに詰める用
	std::unique_ptr<Enemy> enemy_;
	//敵のリスト
	std::list<std::unique_ptr<Enemy>> enemies_;
	//敵の初期最大値
	static const size_t enemyNum_ = 100;
	//追加する敵の数
	const size_t addEnemyNum_ = 1;
	//ひとまず手打ちでの敵の初期値座標
	std::array<Vector3, enemyNum_> enemysPos_;
	//プレイヤーの座標
	const EulerTransform* targetTrans_ = nullptr;
	//プレイヤーの回転
	const Matrix4x4* targetRotateMat_ = nullptr;	
};


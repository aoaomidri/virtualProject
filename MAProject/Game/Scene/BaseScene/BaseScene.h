#pragma once
/*シーンの基本設定*/

//前方宣言
class SceneManager;

class BaseScene{
public:
	enum class SceneName {
		Title,
		Game,
		Result,
	};


	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 3D描画処理
	/// </summary>
	virtual void AllDraw3D() = 0;

	/// <summary>
	/// 2D描画処理
	/// </summary>
	virtual void AllDraw2D() = 0;

	/// <summary>
	/// imguiの処理
	/// </summary>
	virtual void Debug() = 0;
	/// <summary>
	/// シーンのセット
	/// </summary>
	/// <param name="sceneManager"></param>
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

public:
	virtual ~BaseScene() = default;

protected:


private:
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
};


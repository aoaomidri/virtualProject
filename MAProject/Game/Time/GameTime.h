#pragma once
//ゲーム内の時間を管理するクラス

class GameTime{
public:
	static float deltaTime_;
    static float timeScale_;     
    //時間の更新処理
    static void Update();
    //インゲームの時間更新処理
    static void InGameUpdate();

    // 実際の時間経過を取得する関数
    static float GetScaledDeltaTime() {
        return deltaTime_ * timeScale_;
    }
    //スローかどうかを取得
    static bool GetIsSlow() {
        return isSlowDown_;
    } 

    
    /// <summary>
    /// インゲームで経過秒数の一の位を取得(0～59まで)
    /// </summary>
    /// <returns></returns>
    static int GetSecondsOnes() {
        int result = 0;
        result = (int)(inGameTime_) % 60;

        result = result % 10;

        return result;
    }

    /// <summary>
    /// インゲームで経過秒数の十の位を取得(0～59まで)
    /// </summary>
    /// <returns></returns>
    static int GetSecondsTens() {
        int result = 0;
        result = (int)(inGameTime_) % 60;
        result = result / 10;

        return result;
    }

    /// <summary>
   /// インゲームで経過秒数を取得(0～59まで)
   /// </summary>
   /// <returns></returns>
    static int GetMinutes() {
        int result = 0;
        result = (int)(inGameTime_) / 60;

        return result;
    }

    static float GetInGameTimer() {
        return inGameTime_;
    }

    static void ResetGameTimer() {
        inGameTime_ = 0.0f;
    }

    //時間スケールの変更
    static void ChangeTimeScale(float timeScale) {
        timeScale_ = timeScale;
    }
    /// <summary>
    /// ゲームの時間を停止させる
    /// </summary>
    /// <param name="stopTime">止める時間</param>
    static void StopTime(const float stopTime) {
        duration_ = stopTime;
        elapsedTime_ = 0.0f;
        isStop_ = true;
    }
    /// <summary>
    /// ゲームの時間を遅くする
    /// </summary>
    /// <param name="slowTime">遅くする時間</param>
    /// <param name="timeScale">時間の倍率</param>
    static void SlowDownTime(const float slowTime,const float timeScale) {
        duration_ = slowTime;
        timeScale_ = timeScale;
        elapsedTime_ = 0.0f;
        isSlowDown_ = true;
    }
    /// <summary>
    /// 時間の変化を戻す
    /// </summary>
    static void ReverseTimeChange() {
        isSlowDown_ = false;
        isStop_ = false;
    }

private:
    static float duration_;   // シェイクの持続時間
    static float elapsedTime_; // シェイクの経過時間

    //時間を停止させるか
    static bool isStop_;

    //時間を遅くさせるか
    static bool isSlowDown_;

    //インゲームの経過時間
    static float inGameTime_;

};




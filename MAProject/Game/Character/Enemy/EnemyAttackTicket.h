#pragma once
#include <iostream>
#include <vector>
#include<array>
#include"ImGuiManager.h"
//敵が攻撃するために必要なチケット(攻撃権)を管理する

class EnemyAttackTicket{
public:
    //コンストラクタで初期化
    EnemyAttackTicket(int enemyCount, int maxTickets)
        : attackTickets_(enemyCount, maxTickets), maxTickets_(maxTickets){}
    // 攻撃リクエスト
    bool requestAttack(int enemyIndex);

    // 攻撃終了（攻撃アニメーション完了時などに呼ぶ）
    void endAttack() {
        if (activeAttackers_ > 0) {
            activeAttackers_--;
        }
    }

    void DrawImgui() {
        ImGui::Begin("同時攻撃数");
        ImGui::SliderInt("攻撃可能数", &maxActiveAttackers_, 0, 15);
        ImGui::End();
    }
    
    const int GetActiveAttckers() {
        return activeAttackers_;
    }

    // 全ての敵のチケットをリセット
    void resetTickets() {
        std::fill(attackTickets_.begin(), attackTickets_.end(), maxTickets_);
        activeAttackers_ = 0; // 攻撃中の敵もリセット
    }

private:
    std::vector<int> attackTickets_; // 各敵の攻撃回数
    int maxTickets_;                 // 各敵の最大攻撃回数
    static int activeAttackers_;     // 現在攻撃している敵の数
    int maxActiveAttackers_ = 5;     // 最大 10 体まで同時攻撃可能
};


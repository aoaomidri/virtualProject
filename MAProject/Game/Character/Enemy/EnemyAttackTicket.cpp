#include "EnemyAttackTicket.h"

int EnemyAttackTicket::activeAttackers_ = 0;

bool EnemyAttackTicket::requestAttack(int enemyIndex){
    if (enemyIndex < 0 || enemyIndex >= attackTickets_.size()) {
        return false; // 無効なインデックス
    }

    // 10体以上攻撃中なら拒否
    if (activeAttackers_ >= maxActiveAttackers_) {
        return false;
    }

    // チケットが残っていれば攻撃を許可
    if (attackTickets_[enemyIndex] > 0) {
        attackTickets_[enemyIndex]--;  // チケット消費
        activeAttackers_++;            // 攻撃中の敵を増やす
        return true;
    }

    return false; // チケット切れ
}

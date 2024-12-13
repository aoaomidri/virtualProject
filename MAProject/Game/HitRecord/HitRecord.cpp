#include "HitRecord.h"

void HitRecord::AddRecord(uint32_t number){
	//履歴に登録
	record_.push_back(number);
}

bool HitRecord::RecordCheck(uint32_t number){
	//同じものがいるか確認
	return std::any_of(record_.begin(), record_.end(), [number](uint32_t value) {
		return value == number;
	});
}

void HitRecord::Clear(){
	//履歴を抹消
	record_.clear();
}

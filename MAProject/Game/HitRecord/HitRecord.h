#pragma once
#include<vector>
#include <algorithm>
/*当たりを記録して重複を防ぐ*/
class HitRecord{
public:

	/// <summary>
	/// ノックバックの種類
	/// </summary>
	enum KnockbackType {
		Left,
		Right,
		Center,
		Guard
	};

	//履歴追加
	void AddRecord(uint32_t number);

	//履歴チェック
	bool RecordCheck(uint32_t number);

	//履歴抹消
	void Clear();

private:
	//履歴
	std::vector<uint32_t> record_;
};


#pragma once
/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	Vector2 operator+(Vector2 num) {
		Vector2 result{};
		result.x = this->x + num.x;
		result.y = this->y + num.y;

		return result;
	}
};
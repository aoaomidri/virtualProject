#pragma once
/// <summary>
/// 2次元ベクトル
/// </summary>
#define _USE_MATH_DEFINES
#include<math.h>
#include<iostream>

/// 
struct Vector2 final {
	float x;
	float y;

	Vector2 operator+(Vector2 num) {
		Vector2 result{};
		result.x = this->x + num.x;
		result.y = this->y + num.y;

		return result;
	}

	Vector2 operator+(Vector2 num)const {
		Vector2 result{};
		result.x = this->x + num.x;
		result.y = this->y + num.y;

		return result;
	}

	Vector2 operator+=(Vector2 num) {
		this->x += num.x;
		this->y += num.y;

		return *this;
	}

	Vector2 operator-(Vector2 num) {
		Vector2 result{};
		result.x = this->x - num.x;
		result.y = this->y - num.y;

		return result;
	}

	Vector2 operator-(Vector2 num)const {
		Vector2 result{};
		result.x = this->x - num.x;
		result.y = this->y - num.y;

		return result;
	}

	Vector2 operator*(Vector2 num) {
		Vector2 result{};
		result.x = this->x * num.x;
		result.y = this->y * num.y;

		return result;
	}

	Vector2 operator/(Vector2 num) {
		Vector2 result{};
		result.x = this->x / num.x;
		result.y = this->y / num.y;

		return result;
	}

	Vector2 operator=(const Vector2& num) {
		this->x = num.x;
		this->y = num.y;

		return *this;
	}

	bool operator==(const Vector2& num) const {
		return this->x == num.x && this->y == num.y;
	}

	inline bool operator!=(const Vector2& num) const { return !(*this == num); }

	Vector2 operator*(float t) {
		Vector2 result;
		result.x = t * this->x;
		result.y = t * this->y;
		return result;
	}

	Vector2 operator*(float t) const {
		Vector2 result;
		result.x = t * this->x;
		result.y = t * this->y;
		return result;
	}

	Vector2 Add(const Vector2& vA, const Vector2& vB) {
		Vector2 result{ 0 };
		result.x = vA.x + vB.x;
		result.y = vA.y + vB.y;
		return result;
	}

	static Vector2 Mutiply(const Vector2& vec, const float speed) {
		Vector2 result{ 0 };
		result.x = vec.x * speed;
		result.y = vec.y * speed;
		return result;
	};

	static Vector2 Mutiply(const Vector2& vec1, const Vector2& vec2) {
		Vector2 result{ 0 };
		result.x = vec1.x * vec2.x;
		result.y = vec1.y * vec2.y;
		return result;
	};

	static Vector2 Normalize(const Vector2& v) {
		Vector2 result{ 0, 0 };
		float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y)));

		if (bulletNorm != 0.0f) {

			result = { (v.x / bulletNorm), (v.y / bulletNorm) };
		}

		return result;
	}

};
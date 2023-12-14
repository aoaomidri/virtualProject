#pragma once
/// <summary>
/// 3次元ベクトル
/// </summary>
#define _USE_MATH_DEFINES
#include<math.h>
#include<iostream>
class Vector3 {
public:
	float x, y, z;

	Vector3 operator+(Vector3 num) {
		Vector3 result{};
		result.x = this->x + num.x;
		result.y = this->y + num.y;
		result.z = this->z + num.z;

		return result;
	}

	Vector3 operator+(Vector3 num) const {
		Vector3 result{};
		result.x = this->x + num.x;
		result.y = this->y + num.y;
		result.z = this->z + num.z;

		return result;
	}

	Vector3 operator+=(Vector3 num) {
		this->x += num.x;
		this->y += num.y;
		this->z += num.z;

		return *this;
	}

	Vector3 operator-(Vector3 num) {
		Vector3 result{};
		result.x = this->x - num.x;
		result.y = this->y - num.y;
		result.z = this->z - num.z;

		return result;
	}

	Vector3 operator-(Vector3 num)const {
		Vector3 result{};
		result.x = this->x - num.x;
		result.y = this->y - num.y;
		result.z = this->z - num.z;

		return result;
	}

	Vector3 operator*(Vector3 num) {
		Vector3 result{};
		result.x = this->x * num.x;
		result.y = this->y * num.y;
		result.z = this->z * num.z;

		return result;
	}

	Vector3 operator/(Vector3 num) {
		Vector3 result{};
		result.x = this->x / num.x;
		result.y = this->y / num.y;
		result.z = this->z / num.z;

		return result;
	}

	Vector3 operator=(const Vector3& num) {
		this->x = num.x;
		this->y = num.y;
		this->z = num.z;
		return *this;
	}

	bool operator==(const Vector3& num) const {
		return this->x == num.x && this->y == num.y && this->z == num.z;
	}

	inline bool operator!=(const Vector3& num) const { return !(*this == num); }

	Vector3 operator*(float t) {
		Vector3 result;
		result.x = t * this->x;
		result.y = t * this->y;
		result.z = t * this->z;
		return result;
	}

	Vector3 operator*(float t) const {
		Vector3 result;
		result.x = t * this->x;
		result.y = t * this->y;
		result.z = t * this->z;
		return result;
	}

	Vector3 Add(const Vector3& vA, const Vector3& vB) {
		Vector3 result{ 0 };
		result.x = vA.x + vB.x;
		result.y = vA.y + vB.y;
		result.z = vA.z + vB.z;
		return result;
	}

	static Vector3 Mutiply(const Vector3& vec, const float speed) {
		Vector3 result{ 0 };
		result.x = vec.x * speed;
		result.y = vec.y * speed;
		result.z = vec.z * speed;
		return result;
	};

	static Vector3 Mutiply(const Vector3& vec1, const Vector3& vec2) {
		Vector3 result{ 0 };
		result.x = vec1.x * vec2.x;
		result.y = vec1.y * vec2.y;
		result.z = vec1.z * vec2.z;
		return result;
	};

	static Vector3 Cross(const Vector3& vA, const Vector3& vB) {
		Vector3 result{};

		result = {
			(vA.y * vB.z) - (vA.z * vB.y), (vA.z * vB.x) - (vA.x * vB.z),
			(vA.x * vB.y) - (vA.y * vB.x) };

		return result;
	}

	static float Dot(const Vector3& v1, const Vector3& v2) {
		float result = 0;
		result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
		return result;
	}

	static Vector3 Normalize(const Vector3& v) {
		Vector3 result{ 0, 0, 0 };
		float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));

		if (bulletNorm != 0.0f) {

			result = { (v.x / bulletNorm), (v.y / bulletNorm), (v.z / bulletNorm) };
		}

		return result;
	}

	static float LerpShortAngle(float a, float b, float t) {
		float diff = b - a;

		diff = static_cast<float>(std::fmod(diff, 2.0 * static_cast<float>(M_PI)));

		if (diff > static_cast<float>(M_PI)) {
			diff -= 2.0 * static_cast<float>(M_PI);
		}
		else if (diff < static_cast<float>(M_PI) * -1) {
			diff += 2.0 * static_cast<float>(M_PI);
		}

		return a + diff * t;

	}

	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
		Vector3 result{ 0, 0, 0 };
		result = v1 + (v2 - v1) * t;
		return result;
	}

	static float Length(const Vector3& v) {
		float result{ 0.0f };
		float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
		result = bulletNorm;
		return result;
	}

private:
};

/// <summary>
/// 線分
/// </summary>
struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};


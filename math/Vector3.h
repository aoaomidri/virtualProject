#pragma once
/// <summary>
/// 3次元ベクトル
/// </summary>
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

	Vector3 Mutiply(const Vector3& vec, const float speed) {
		Vector3 result{ 0 };
		result.x = vec.x * speed;
		result.y = vec.y * speed;
		result.z = vec.z * speed;
		return result;
	};

	Vector3 Cross(const Vector3& vA, const Vector3& vB) {
		Vector3 result{};

		result = {
			(vA.y * vB.z) - (vA.z * vB.y), (vA.z * vB.x) - (vA.x * vB.z),
			(vA.x * vB.y) - (vA.y * vB.x) };

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
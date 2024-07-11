#pragma once
#pragma once
#include<iostream>
#include"Vector3.h"
/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4 {
public:
	float x, y, z, w;

	Vector4& operator=(const Vector3& num) {
		this->x = num.x;
		this->y = num.y;
		this->z = num.z;
		this->w = 1.0f;
		return *this;
	}
};


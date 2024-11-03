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

	Vector4& operator=(const Vector3& num);

	
};




#pragma once
#include"Vector3.h"
#include"Vector4.h"
class Quaternion{
public:
	Quaternion();
	~Quaternion();

	static Quaternion* GetInstance();
public:
	//Quaternionの積
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
	//単位Quaternionを返す
	Quaternion IdentityQuaternion();
	//共役Quaternionを返す
	Quaternion Conjugate(const Quaternion& quaternion);
	//Quaternionのnormを返す
	float Norm(const Quaternion& quaternion);
	//正規化したQuaternionを返す
	Quaternion Normalize(const Quaternion& quaternion);
	//逆Quaternionを返す
	Quaternion Inverse(const Quaternion& quaternion);

public:

	Vector4 quaternion_;
	//x,y,z
	Vector3 vector_;
	//w
	float w;
};


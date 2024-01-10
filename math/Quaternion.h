#pragma once
#include"Matrix.h"
class Quaternion{
public:
	Quaternion();
	~Quaternion();

	static Quaternion* GetInstance();
public:
	//Quaternionの和
	Quaternion Add(const Quaternion& q0, const Quaternion& q1);
	//Quaternionの積
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
	//Quaternionの積
	Quaternion Multiply(const Quaternion& quaternion, const float n);
	//単位Quaternionを返す
	Quaternion IdentityQuaternion();
	//共役Quaternionを返す
	Quaternion Conjugate(const Quaternion& quaternion);
	//Quaternionのnormを返す
	float Norm(const Quaternion& quaternion);
	//Quaternionの内積を返す
	float Dot(const Quaternion& q0, const Quaternion& q1);
	//反転Quaternionを返す
	Quaternion Reverse(const Quaternion& quaternion);
	//正規化したQuaternionを返す
	Quaternion Normalize(const Quaternion& quaternion);
	//逆Quaternionを返す
	Quaternion Inverse(const Quaternion& quaternion);
	//任意軸を表すQuaternionの生成
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vector, const float angle);
	//ベクトルをQuaternionで回転させた結果のベクトルを求める
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	//Quaternionから回転行列を求める
	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
	//球面線形補間
	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

public:

	Vector4 quaternion_;

private:
	//x,y,z
	Vector3 vector_;
	//w
	float w;
};


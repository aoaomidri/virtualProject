#pragma once
#include"Matrix4x4.h"
/*クウォータニオンを計算する*/
class Quaternion{
public:
	Quaternion();
	Quaternion(const Vector4& vec);
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
	Quaternion Slerp(Quaternion q0, const Quaternion& q1, float t);

	//Matrixをクウォータニオンに変換
	Quaternion FromMatrix(const Matrix4x4& mat);
	//球面線形補間(Matrixをクウォータニオンに変換し、補完を掛けた後に戻す)
	Matrix4x4 Slerp(Matrix4x4 m0, const Matrix4x4& m1, float t);

	//球面線形補間(Matrixをクウォータニオンに変換し、補完を掛けた後に戻す)
	Matrix4x4 Slerp(Vector3 m0, const Vector3& m1, float t);

public:

	Vector4 quaternion_;

private:
	//x,y,z
	Vector3 vector_;
	//w
	float w;
};


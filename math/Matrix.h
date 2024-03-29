#pragma once
#include"../Engine/3D/Transform.h"
#include<math.h>


struct Matrix3x3{
	float m[3][3];
};

class Matrix {
public:
	Matrix();

	static Matrix* GetInstance();

	Matrix4x4 m;
private:
	Vector3 rotate;

	Vector3 translate;

	Matrix4x4 ScaleMatrix;	

	Matrix4x4 TranslateMatrix;

	Matrix4x4 RotateMatrixXYZ;
	Matrix4x4 RotateMatrixX;
	Matrix4x4 RotateMatrixY;
	Matrix4x4 RotateMatrixZ;

	Matrix4x4 worldMatrix;
	Matrix4x4 cameraMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4 viewportMatrix;
	Vector3 kLocalVertices[3];

	Vector3 v1;

	Vector3 v2;

	

public:
	Vector3 cross;
	Vector3 screenVertices[3];


public:
	void Update();

	Vector3 Cross(const Vector3& vA, const Vector3& vB);

	Matrix4x4 Minus(const Matrix4x4& m1, const Matrix4x4& m2);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Vector3& rot, const Vector3& translate_);
	//マトリックスから作るアフィン行列
	Matrix4x4 MakeAffineMatrix(const Matrix4x4& scale, const Matrix4x4& rot, const Matrix4x4& translate);

	//トランスフォームから作るアフィン行列
	Matrix4x4 MakeAffineMatrix(const Transform& transform);

	Matrix4x4 MakeScaleMatrix(const Vector3& scale_);

	Matrix4x4 MakeRotateMatrixX(const Vector3& rot);
	Matrix4x4 MakeRotateMatrixY(const Vector3& rot);
	Matrix4x4 MakeRotateMatrixZ(const Vector3& rot);

	Matrix4x4 MakeRotateMatrix(const Vector3& rot);
	

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate_);

	////積
	Matrix4x4 Multiply(const Matrix4x4& mat1, const Matrix4x4& mat2);

	//正規化
	Vector3 Normalize(const Vector3& v);

	//透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	//正射影行列(平行投影行列)
	Matrix4x4 MakeOrthographicMatrix(float left,float top ,float right, float bottom, float nearClip, float farClip);
	//ビューポート行列
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	Matrix4x4 MakeIdentity4x4();

	//任意軸回転行列
	Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

	////逆行列
	Matrix4x4 Inverce(const Matrix4x4& mat);

	//ベクトル変換
	Vector3 TransformVec(const Vector3& v, const Matrix4x4& m);

	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	float RotateAngleYFromMatrix(const Matrix4x4& m);

	Matrix4x4 operator+(const Matrix4x4& mat) const;

	Matrix4x4& operator+=(const Matrix4x4& mat);

	Matrix4x4 operator-(const Matrix4x4& mat) const;

	Matrix4x4& operator-=(const Matrix4x4& mat);

	Matrix4x4 operator*(const Matrix4x4& mat) const;

	Matrix4x4& operator*=(const Matrix4x4& mat);

	Matrix4x4& operator=(const Matrix4x4& mat);

};

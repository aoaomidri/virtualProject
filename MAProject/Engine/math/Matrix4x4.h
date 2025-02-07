#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include<DirectXMath.h>
#include<array>
/*行列の計算をする*/

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

class Matrix4x4{
	//関数軍
public:
	Matrix4x4();
	Matrix4x4(const Matrix4x4& mat);
	Matrix4x4(std::initializer_list<std::initializer_list<float>> list);
	
	//単位行列化
	Matrix4x4 Identity();
	//単位行列化
    static Matrix4x4 StaticIdentity();

	//0で初期化
	Matrix4x4 MatInit();

	//Matrixの引き算
	Matrix4x4 Minus(const Matrix4x4& right) const;
	//Matrixの掛け算
	Matrix4x4 Multiply(const Matrix4x4& right) const;
	//逆行列の作成
	Matrix4x4 Inverce();


	//Scale成分の逆行列
	Matrix4x4 ScaleInverce();
	//転置行列の作成
	Matrix4x4 Trnaspose();

	Matrix4x4 MakeScaleMatrix(const Vector3& scale_);

	Matrix4x4 MakeRotateMatrixX(const Vector3& rot);
	Matrix4x4 MakeRotateMatrixY(const Vector3& rot);
	Matrix4x4 MakeRotateMatrixZ(const Vector3& rot);

	Matrix4x4 MakeRotateMatrix(const Vector3& rot);

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate_);
	
	Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Vector3& rot, const Vector3& translate_);
	//マトリックスから作るアフィン行列
	Matrix4x4 MakeAffineMatrix(const Matrix4x4& scale, const Matrix4x4& rot, const Matrix4x4& translate);

	//トランスフォームから作るアフィン行列
	Matrix4x4 MakeAffineMatrix(const EulerTransform& transform);

	//透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	//正射影行列(平行投影行列)
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	//ビューポート行列
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	//任意軸回転行列
	Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
	//方向ベクトルから行列を生成
	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

	//ベクトル変換
	Vector3 TransformVec(const Vector3& v);
	//3Dベクトルを行列で変換する
	Vector3 TransformNormal(const Vector3& v);
	//X軸の値を抽出
	float RotateAngleXFromMatrix() const;
	//Y軸の値を抽出
	float RotateAngleYFromMatrix() const;
	//Z軸の値を抽出
	float RotateAngleZFromMatrix() const;
	//XYZ軸の値を抽出
	Vector3 RotateAngleFromMatrix() const;
	//位置情報を抽出
	const Vector3 GetTranslate()const {
		Vector3 translate{};
		translate = {
			m[3][0],
			m[3][1],
			m[3][2]
		};

		return translate;
	}

private:
	//ベクトルの長さと符合を計算する関数
	float CalculateLengthWithSign(const std::array<float, 3>& v);
	//オペレーター
public:
	Matrix4x4 operator+(const Matrix4x4& mat) const;

	Matrix4x4 operator+=(const Matrix4x4& mat);

	Matrix4x4 operator-(const Matrix4x4& mat) const;

	Matrix4x4 operator-=(const Matrix4x4& mat);

	Matrix4x4 operator*(const Matrix4x4& mat) const;

	Matrix4x4 operator*=(const Matrix4x4& mat);

	constexpr Matrix4x4& operator=(const Matrix4x4&) = default;
	constexpr Matrix4x4& operator=(Matrix4x4&&) = default;

public:
	float m[4][4];
};



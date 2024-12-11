#pragma once
#include"Transform.h"
#include"Quaternion.h"
#include<math.h>
#include<DirectXMath.h>
/*行列の計算をする「matrix4x4と同義で本来必要がないので削除予定」*/


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
	//クロス積
	static Vector3 Cross(const Vector3& vA, const Vector3& vB);
	//引き算
	static Matrix4x4 Minus(const Matrix4x4& m1, const Matrix4x4& m2);
	//vector3のみのアフィン行列
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Vector3& rot, const Vector3& translate_);
	//回転行列を用いたアフィン行列
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Matrix4x4& rot, const Vector3& translate_);
	//Quaternionを用いたアフィン行列
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Quaternion& rot, const Vector3& translate_);
	//マトリックスから作るアフィン行列
	static Matrix4x4 MakeAffineMatrix(const Matrix4x4& scale, const Matrix4x4& rot, const Matrix4x4& translate);

	//トランスフォームから作るアフィン行列
	static Matrix4x4 MakeAffineMatrix(const EulerTransform& transform);
	//クウォータニオントランスフォームから作るアフィン行列
	static Matrix4x4 MakeAffineMatrix(const QuaternionTransform& transform);
	//スケールの行列
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale_);
	/*それぞれの角度の回転行列*/
	static Matrix4x4 MakeRotateMatrixX(const Vector3& rot);
	static Matrix4x4 MakeRotateMatrixY(const Vector3& rot);
	static Matrix4x4 MakeRotateMatrixZ(const Vector3& rot);
	//回転行列
	static Matrix4x4 MakeRotateMatrix(const Vector3& rot);
	
	//移動行列
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate_);

	////積
	static Matrix4x4 Multiply(const Matrix4x4& mat1, const Matrix4x4& mat2);

	//正規化
	static Vector3 Normalize(const Vector3& v);

	//透視投影行列
	static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	//正射影行列(平行投影行列)
	static Matrix4x4 MakeOrthographicMatrix(float left,float top ,float right, float bottom, float nearClip, float farClip);
	//ビューポート行列
	static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	//単位行列の生成
	static Matrix4x4 MakeIdentity4x4();

	//任意軸回転行列
	static Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

	////逆行列
	static Matrix4x4 Inverce(const Matrix4x4& mat);
	//行列の要素を反転
	static Matrix4x4 Transpose(const Matrix4x4& mat);

	//ベクトル変換
	static Vector3 TransformVec(const Vector3& v, const Matrix4x4& m);
	//3Dベクトルを行列で変換する
	static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
	//Y軸の値を抽出
	static float RotateAngleYFromMatrix(const Matrix4x4& m);


};

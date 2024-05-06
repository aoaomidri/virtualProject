#include "Matrix.h"
#include<assert.h>
#include<stdint.h>
#include <xkeycheck.h>

Matrix::Matrix(){
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.m[i][j] = 0.0f;
			ScaleMatrix.m[i][j] = 0.0f;
			TranslateMatrix.m[i][j] = 0.0f;
			RotateMatrixXYZ.m[i][j] = 0.0f;
			RotateMatrixX.m[i][j] = 0.0f;
			RotateMatrixY.m[i][j] = 0.0f;
			RotateMatrixZ.m[i][j] = 0.0f;
			worldMatrix.m[i][j] = 0.0f;
			cameraMatrix.m[i][j] = 0.0f;
			viewMatrix.m[i][j] = 0.0f;
			projectionMatrix.m[i][j] = 0.0f;
			worldViewProjectionMatrix.m[i][j] = 0.0f;
			viewportMatrix.m[i][j] = 0.0f;
		}
	}

	for (int i = 0; i < 3; i++){
		screenVertices[i] = { 0.0f,0.0f,0.0f };
	}
	kLocalVertices[0] = { 0.0f,1.5f,0.0f, };
	kLocalVertices[1] = { 1.5f,-1.5f,0.0f, };
	kLocalVertices[2] = { -1.5f,-1.5f,0.0f, };
	rotate = { 0.0f,0.0f,0.0f };
	translate = { 0.0f,0.0f,0.0f };
	cross = { 0.0f,0.0f,0.0f };
	v1 = { 1.2f,-3.9f,2.5f };
	v2 = { 2.8f,0.4f,-1.3f };
}
Matrix* Matrix::GetInstance(){
	static Matrix instance;
	return &instance;
}
;

void Matrix::Update() {
	
}

Vector3 Matrix::Cross(const Vector3& vA, const Vector3& vB) {
	Vector3 result{};

	result = {
		(vA.y * vB.z) - (vA.z * vB.y),
		(vA.z * vB.x) - (vA.x * vB.z),
		(vA.x * vB.y) - (vA.y * vB.x)
	};

	return result;
}

Matrix4x4 Matrix::Minus(const Matrix4x4& m1, const Matrix4x4& m2){
	Matrix4x4 result{};
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			result.m[y][x] = m1.m[y][x] - m2.m[y][x];
		}
	}
	return result;
}


////積
Matrix4x4 Matrix::Multiply(const Matrix4x4& mat1, const Matrix4x4& mat2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = mat1.m[i][0] * mat2.m[0][j] + mat1.m[i][1] * mat2.m[1][j] +
			                 mat1.m[i][2] * mat2.m[2][j] + mat1.m[i][3] * mat2.m[3][j];
		}
	}
	return result;
}

Matrix4x4 Matrix::Inverce(const Matrix4x4& mat) {
	float A = 0.0f;
	Matrix4x4 result = {};
	//行列式の計算
	A = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3] +
		mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1] +
		mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2] -
		mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1] -
		mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3] -
		mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2] -
		mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3] -
		mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1] -
		mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2] +
		mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1] +
		mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3] +
		mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2] +
		mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3] +
		mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1] +
		mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2] -
		mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1] -
		mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3] -
		mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2] -
		mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0] -
		mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0] -
		mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0] +
		mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0] +
		mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0] +
		mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0];

	//逆行列の計算
	result.m[0][0] = (mat.m[1][1] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][1] + mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
		- mat.m[1][3] * mat.m[2][2] * mat.m[3][1] - mat.m[1][2] * mat.m[2][1] * mat.m[3][3] - mat.m[1][1] * mat.m[2][3] * mat.m[3][2]) / A;

	result.m[0][1] = (-mat.m[0][1] * mat.m[2][2] * mat.m[3][3] - mat.m[0][2] * mat.m[2][3] * mat.m[3][1] - mat.m[0][3] * mat.m[2][1] * mat.m[3][2]
		+ mat.m[0][3] * mat.m[2][2] * mat.m[3][1] + mat.m[0][2] * mat.m[2][1] * mat.m[3][3] + mat.m[0][1] * mat.m[2][3] * mat.m[3][2]) / A;

	result.m[0][2] = (mat.m[0][1] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][1] + mat.m[0][3] * mat.m[1][1] * mat.m[3][2]
		- mat.m[0][3] * mat.m[1][2] * mat.m[3][1] - mat.m[0][2] * mat.m[1][1] * mat.m[3][3] - mat.m[0][1] * mat.m[1][3] * mat.m[3][2]) / A;

	result.m[0][3] = (-mat.m[0][1] * mat.m[1][2] * mat.m[2][3] - mat.m[0][2] * mat.m[1][3] * mat.m[2][1] - mat.m[0][3] * mat.m[1][1] * mat.m[2][2]
		+ mat.m[0][3] * mat.m[1][2] * mat.m[2][1] + mat.m[0][2] * mat.m[1][1] * mat.m[2][3] + mat.m[0][1] * mat.m[1][3] * mat.m[2][2]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[1][0] = (-mat.m[1][0] * mat.m[2][2] * mat.m[3][3] - mat.m[1][2] * mat.m[2][3] * mat.m[3][0] - mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
		+ mat.m[1][3] * mat.m[2][2] * mat.m[3][0] + mat.m[1][2] * mat.m[2][0] * mat.m[3][3] + mat.m[1][0] * mat.m[2][3] * mat.m[3][2]) / A;

	result.m[1][1] = (mat.m[0][0] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][0] + mat.m[0][3] * mat.m[2][0] * mat.m[3][2]
		- mat.m[0][3] * mat.m[2][2] * mat.m[3][0] - mat.m[0][2] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][2]) / A;

	result.m[1][2] = (-mat.m[0][0] * mat.m[1][2] * mat.m[3][3] - mat.m[0][2] * mat.m[1][3] * mat.m[3][0] - mat.m[0][3] * mat.m[1][0] * mat.m[3][2]
		+ mat.m[0][3] * mat.m[1][2] * mat.m[3][0] + mat.m[0][2] * mat.m[1][0] * mat.m[3][3] + mat.m[0][0] * mat.m[1][3] * mat.m[3][2]) / A;

	result.m[1][3] = (mat.m[0][0] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][0] + mat.m[0][3] * mat.m[1][0] * mat.m[2][2]
		- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] - mat.m[0][2] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][2]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[2][0] = (mat.m[1][0] * mat.m[2][1] * mat.m[3][3] + mat.m[1][1] * mat.m[2][3] * mat.m[3][0] + mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
		- mat.m[1][3] * mat.m[2][1] * mat.m[3][0] - mat.m[1][1] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][1]) / A;

	result.m[2][1] = (-mat.m[0][0] * mat.m[2][1] * mat.m[3][3] - mat.m[0][1] * mat.m[2][3] * mat.m[3][0] - mat.m[0][3] * mat.m[2][0] * mat.m[3][1]
		+ mat.m[0][3] * mat.m[2][1] * mat.m[3][0] + mat.m[0][1] * mat.m[2][0] * mat.m[3][3] + mat.m[0][0] * mat.m[2][3] * mat.m[3][1]) / A;

	result.m[2][2] = (mat.m[0][0] * mat.m[1][1] * mat.m[3][3] + mat.m[0][1] * mat.m[1][3] * mat.m[3][0] + mat.m[0][3] * mat.m[1][0] * mat.m[3][1]
		- mat.m[0][3] * mat.m[1][1] * mat.m[3][0] - mat.m[0][1] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][1]) / A;

	result.m[2][3] = (-mat.m[0][0] * mat.m[1][1] * mat.m[2][3] - mat.m[0][1] * mat.m[1][3] * mat.m[2][0] - mat.m[0][3] * mat.m[1][0] * mat.m[2][1]
		+ mat.m[0][3] * mat.m[1][1] * mat.m[2][0] + mat.m[0][1] * mat.m[1][0] * mat.m[2][3] + mat.m[0][0] * mat.m[1][3] * mat.m[2][1]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[3][0] = (-mat.m[1][0] * mat.m[2][1] * mat.m[3][2] - mat.m[1][1] * mat.m[2][2] * mat.m[3][0] - mat.m[1][2] * mat.m[2][0] * mat.m[3][1]
		+ mat.m[1][2] * mat.m[2][1] * mat.m[3][0] + mat.m[1][1] * mat.m[2][0] * mat.m[3][2] + mat.m[1][0] * mat.m[2][2] * mat.m[3][1]) / A;

	result.m[3][1] = (mat.m[0][0] * mat.m[2][1] * mat.m[3][2] + mat.m[0][1] * mat.m[2][2] * mat.m[3][0] + mat.m[0][2] * mat.m[2][0] * mat.m[3][1]
		- mat.m[0][2] * mat.m[2][1] * mat.m[3][0] - mat.m[0][1] * mat.m[2][0] * mat.m[3][2] - mat.m[0][0] * mat.m[2][2] * mat.m[3][1]) / A;

	result.m[3][2] = (-mat.m[0][0] * mat.m[1][1] * mat.m[3][2] - mat.m[0][1] * mat.m[1][2] * mat.m[3][0] - mat.m[0][2] * mat.m[1][0] * mat.m[3][1]
		+ mat.m[0][2] * mat.m[1][1] * mat.m[3][0] + mat.m[0][1] * mat.m[1][0] * mat.m[3][2] + mat.m[0][0] * mat.m[1][2] * mat.m[3][1]) / A;

	result.m[3][3] = (mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] + mat.m[0][2] * mat.m[1][0] * mat.m[2][1]
		- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1]) / A;

	return result;
}

Matrix4x4 Matrix::Trnaspose(const Matrix4x4& mat) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = mat.m[j][i];
		}
	}
	return result;
}



Matrix4x4 Matrix::MakeScaleMatrix(const Vector3& scale_){
	Matrix4x4 result{};
	result.m[0][0] = scale_.x;
	result.m[1][1] = scale_.y;
	result.m[2][2] = scale_.z;
	result.m[3][3] = 1.0f;
	return result;
}
Matrix4x4 Matrix::MakeRotateMatrixX(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = cosf(rot.x);
	result.m[1][2] = sinf(rot.x);
	result.m[2][1] = -sinf(rot.x);
	result.m[2][2] = cosf(rot.x);
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Matrix::MakeRotateMatrixY(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[1][1] = 1.0f;
	result.m[0][0] = cosf(rot.y);
	result.m[0][2] = -sinf(rot.y);
	result.m[2][0] = sinf(rot.y);
	result.m[2][2] = cosf(rot.y);
	result.m[3][3] = 1.0f;
	return result;
}
Matrix4x4 Matrix::MakeRotateMatrixZ(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[2][2] = 1.0f;
	result.m[0][0] = cosf(rot.z);
	result.m[0][1] = sinf(rot.z);
	result.m[1][0] = -sinf(rot.z);
	result.m[1][1] = cosf(rot.z);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 Matrix::MakeRotateMatrix(const Vector3& rot) {
	Matrix4x4 result{};
	// X,Y,Z軸の回転行列の作成
	RotateMatrixX = MakeRotateMatrixX(rot);
	RotateMatrixY = MakeRotateMatrixY(rot);
	RotateMatrixZ = MakeRotateMatrixZ(rot);

	result = Multiply(RotateMatrixX, Multiply(RotateMatrixY, RotateMatrixZ));

	return result;
}

Matrix4x4 Matrix::MakeTranslateMatrix(const Vector3& translate_) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[3][0] = translate_.x;
	result.m[3][1] = translate_.y;
	result.m[3][2] = translate_.z;
	return result;

}

Matrix4x4
    Matrix::MakeAffineMatrix(const Vector3& scale_, const Vector3& rot, const Vector3& translate_) {
	Matrix4x4 result{};

	//スケーリング行列の作成
	ScaleMatrix = MakeScaleMatrix(scale_);
	//X,Y,Z軸の回転行列の作成
	RotateMatrixX = MakeRotateMatrixX(rot);
	RotateMatrixY = MakeRotateMatrixY(rot);
	RotateMatrixZ = MakeRotateMatrixZ(rot);
	//回転行列の結合
	RotateMatrixXYZ = Multiply(RotateMatrixX, Multiply(RotateMatrixY, RotateMatrixZ));
	//平行移動行列の作成
	TranslateMatrix = MakeTranslateMatrix(translate_);

	result = Multiply(ScaleMatrix, Multiply(RotateMatrixXYZ, TranslateMatrix));

	return result;

}

Matrix4x4 Matrix::MakeAffineMatrix(const Vector3& scale_, const Quaternion& rot, const Vector3& translate_){
	Matrix4x4 result{};

	//スケーリング行列の作成
	ScaleMatrix = MakeScaleMatrix(scale_);
	//正規化したQuaternionの作成
	//Quaternion normQua = Quaternion::GetInstance()->Normalize(rot);
	//X,Y,Z軸の回転行列の作成
	RotateMatrixXYZ = Quaternion::GetInstance()->MakeRotateMatrix(rot);
	//平行移動行列の作成
	TranslateMatrix = MakeTranslateMatrix(translate_);

	result = Multiply(ScaleMatrix, Multiply(RotateMatrixXYZ, TranslateMatrix));

	return result;
}

Matrix4x4 Matrix::MakeAffineMatrix(const Matrix4x4& scale, const Matrix4x4& rot, const Matrix4x4& translate){
	Matrix4x4 result{};
	result = Multiply(scale, Multiply(rot, translate));
	return result;
}

Matrix4x4 Matrix::MakeAffineMatrix(const EulerTransform& transform){
	Matrix4x4 result{};

	//スケーリング行列の作成
	ScaleMatrix = MakeScaleMatrix(transform.scale);
	//X,Y,Z軸の回転行列の作成
	RotateMatrixX = MakeRotateMatrixX(transform.rotate);
	RotateMatrixY = MakeRotateMatrixY(transform.rotate);
	RotateMatrixZ = MakeRotateMatrixZ(transform.rotate);
	//回転行列の結合
	RotateMatrixXYZ = Multiply(RotateMatrixX, Multiply(RotateMatrixY, RotateMatrixZ));
	//平行移動行列の作成
	TranslateMatrix = MakeTranslateMatrix(transform.translate);

	result = Multiply(ScaleMatrix, Multiply(RotateMatrixXYZ, TranslateMatrix));

	return result;
}

Matrix4x4 Matrix::MakeAffineMatrix(const QuaternionTransform& transform){
	return MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
}

Vector3 Matrix::Normalize(const Vector3& v) {
	Vector3 result{0, 0, 0};
	float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));

	if (bulletNorm != 0.0f) {

		result = {
		    (v.x / bulletNorm),
			(v.y / bulletNorm),
		    (v.z / bulletNorm)
		};
	}

	return result;

}

Vector3 Matrix::TransformVec(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2]
	};
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Vector3 Matrix::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};

	return result;
}

float Matrix::RotateAngleYFromMatrix(const Matrix4x4& m){
	float angle = std::acos(m.m[0][0]);
	if (m.m[2][0] < 0) {
		angle = -angle;  // acosの結果だけでは回転の向きがわからないので符号を調整
	}

	return angle;
}


Matrix4x4 Matrix::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = (1.0f / aspectRatio) * (1.0f / tanf(fovY / 2.0f));
	result.m[1][1] = (1.0f / tanf(fovY / 2.0f));
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

Matrix4x4 Matrix::MakeOrthographicMatrix(float left,float top ,float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Matrix::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result{};

	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -(height / 2.0f);
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Matrix::MakeIdentity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

Matrix4x4 Matrix::MakeRotateAxisAngle(const Vector3& axis, float angle){
	Matrix4x4 matrix{};
	matrix.m[0][0] = (axis.x * axis.x) * (1 - std::cosf(angle)) + std::cosf(angle);
	matrix.m[0][1] = (axis.x * axis.y) * (1 - std::cosf(angle)) + (axis.z * std::sinf(angle));
	matrix.m[0][2] = (axis.x * axis.z) * (1 - std::cosf(angle)) - (axis.y * std::sinf(angle));
	matrix.m[1][0] = (axis.x * axis.y) * (1 - std::cosf(angle)) - (axis.z * std::sinf(angle));
	matrix.m[1][1] = (axis.y * axis.y) * (1 - std::cosf(angle)) + std::cosf(angle);
	matrix.m[1][2] = (axis.y * axis.z) * (1 - std::cosf(angle)) + (axis.x * std::sinf(angle));
	matrix.m[2][0] = (axis.x * axis.z) * (1 - std::cosf(angle)) + (axis.y * std::sinf(angle));
	matrix.m[2][1] = (axis.y * axis.z) * (1 - std::cosf(angle)) - (axis.x * std::sinf(angle));
	matrix.m[2][2] = (axis.z * axis.z) * (1 - std::cosf(angle)) + std::cosf(angle);
	matrix.m[3][3] = 1.0f;
	return matrix;
}


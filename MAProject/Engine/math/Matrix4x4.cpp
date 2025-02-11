#include"Matrix4x4.h"
#include <cassert>


Matrix4x4::Matrix4x4(){
	MatInit();
}

Matrix4x4::Matrix4x4(const Matrix4x4& mat){
	*this = mat;
}

Matrix4x4::Matrix4x4(std::initializer_list<std::initializer_list<float>> list){
	if (list.size() != 4)
		throw std::invalid_argument("Initializer list must contain 4 rows.");

	int row_index = 0;
	for (const auto& row : list) {
		if (row.size() != 4)
			throw std::invalid_argument("Each row must contain 4 elements.");

		int col_index = 0;
		for (const auto& element : row) {
			m[row_index][col_index] = element;
			col_index++;
		}
		row_index++;
	}
}

Matrix4x4 Matrix4x4::Identity(){
	for (int i = 0; i < 4; i++) {
		m[i][i] = 1.0f;
	}
	return *this;
}

Matrix4x4 Matrix4x4::StaticIdentity(){
	Matrix4x4 result{};

	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

Matrix4x4 Matrix4x4::MatInit(){
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = 0.0f;
		}
	}
	return *this;
}

Matrix4x4 Matrix4x4::Minus(const Matrix4x4& right) const{
	return *this - right;
}

Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& right) const{
	return *this * right;
}

Matrix4x4 Matrix4x4::Inverce(){
	float A = 0.0f;
	Matrix4x4 result = {};

	//行列式の計算
	A = m[0][0] * m[1][1] * m[2][2] * m[3][3] +
		m[0][0] * m[1][2] * m[2][3] * m[3][1] +
		m[0][0] * m[1][3] * m[2][1] * m[3][2] -
		m[0][0] * m[1][3] * m[2][2] * m[3][1] -
		m[0][0] * m[1][2] * m[2][1] * m[3][3] -
		m[0][0] * m[1][1] * m[2][3] * m[3][2] -
		m[0][1] * m[1][0] * m[2][2] * m[3][3] -
		m[0][2] * m[1][0] * m[2][3] * m[3][1] -
		m[0][3] * m[1][0] * m[2][1] * m[3][2] +
		m[0][3] * m[1][0] * m[2][2] * m[3][1] +
		m[0][2] * m[1][0] * m[2][1] * m[3][3] +
		m[0][1] * m[1][0] * m[2][3] * m[3][2] +
		m[0][1] * m[1][2] * m[2][0] * m[3][3] +
		m[0][2] * m[1][3] * m[2][0] * m[3][1] +
		m[0][3] * m[1][1] * m[2][0] * m[3][2] -
		m[0][3] * m[1][2] * m[2][0] * m[3][1] -
		m[0][2] * m[1][1] * m[2][0] * m[3][3] -
		m[0][1] * m[1][3] * m[2][0] * m[3][2] -
		m[0][1] * m[1][2] * m[2][3] * m[3][0] -
		m[0][2] * m[1][3] * m[2][1] * m[3][0] -
		m[0][3] * m[1][1] * m[2][2] * m[3][0] +
		m[0][3] * m[1][2] * m[2][1] * m[3][0] +
		m[0][2] * m[1][1] * m[2][3] * m[3][0] +
		m[0][1] * m[1][3] * m[2][2] * m[3][0];

	//逆行列の計算
	result.m[0][0] = (m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2]
		- m[1][3] * m[2][2] * m[3][1] - m[1][2] * m[2][1] * m[3][3] - m[1][1] * m[2][3] * m[3][2]) / A;

	result.m[0][1] = (-m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]
		+ m[0][3] * m[2][2] * m[3][1] + m[0][2] * m[2][1] * m[3][3] + m[0][1] * m[2][3] * m[3][2]) / A;

	result.m[0][2] = (m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2]
		- m[0][3] * m[1][2] * m[3][1] - m[0][2] * m[1][1] * m[3][3] - m[0][1] * m[1][3] * m[3][2]) / A;

	result.m[0][3] = (-m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]
		+ m[0][3] * m[1][2] * m[2][1] + m[0][2] * m[1][1] * m[2][3] + m[0][1] * m[1][3] * m[2][2]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[1][0] = (-m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]
		+ m[1][3] * m[2][2] * m[3][0] + m[1][2] * m[2][0] * m[3][3] + m[1][0] * m[2][3] * m[3][2]) / A;

	result.m[1][1] = (m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2]
		- m[0][3] * m[2][2] * m[3][0] - m[0][2] * m[2][0] * m[3][3] - m[0][0] * m[2][3] * m[3][2]) / A;

	result.m[1][2] = (-m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]
		+ m[0][3] * m[1][2] * m[3][0] + m[0][2] * m[1][0] * m[3][3] + m[0][0] * m[1][3] * m[3][2]) / A;

	result.m[1][3] = (m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2]
		- m[0][3] * m[1][2] * m[2][0] - m[0][2] * m[1][0] * m[2][3] - m[0][0] * m[1][3] * m[2][2]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[2][0] = (m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1]
		- m[1][3] * m[2][1] * m[3][0] - m[1][1] * m[2][0] * m[3][3] - m[1][0] * m[2][3] * m[3][1]) / A;

	result.m[2][1] = (-m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]
		+ m[0][3] * m[2][1] * m[3][0] + m[0][1] * m[2][0] * m[3][3] + m[0][0] * m[2][3] * m[3][1]) / A;

	result.m[2][2] = (m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1]
		- m[0][3] * m[1][1] * m[3][0] - m[0][1] * m[1][0] * m[3][3] - m[0][0] * m[1][3] * m[3][1]) / A;

	result.m[2][3] = (-m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]
		+ m[0][3] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][3] + m[0][0] * m[1][3] * m[2][1]) / A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[3][0] = (-m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1]
		+ m[1][2] * m[2][1] * m[3][0] + m[1][1] * m[2][0] * m[3][2] + m[1][0] * m[2][2] * m[3][1]) / A;

	result.m[3][1] = (m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1]
		- m[0][2] * m[2][1] * m[3][0] - m[0][1] * m[2][0] * m[3][2] - m[0][0] * m[2][2] * m[3][1]) / A;

	result.m[3][2] = (-m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]
		+ m[0][2] * m[1][1] * m[3][0] + m[0][1] * m[1][0] * m[3][2] + m[0][0] * m[1][2] * m[3][1]) / A;

	result.m[3][3] = (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
		- m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1]) / A;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			
			m[y][x] = result.m[y][x];
		}
	}
	return *this;
}

Matrix4x4 Matrix4x4::ScaleInverce(){
	Matrix4x4 result{};

	Vector3 scale{};

	scale.x = CalculateLengthWithSign({ m[0][0], m[1][0], m[2][0] });
	scale.y = CalculateLengthWithSign({ m[0][1], m[1][1], m[2][1] });
	scale.z = CalculateLengthWithSign({ m[0][2], m[1][2], m[2][2] });

	result.MakeScaleMatrix(scale);

	result.Inverce();

	return result;
}

Matrix4x4 Matrix4x4::Trnaspose(){
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = m[j][i];
		}
	}
	return *this;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale_){
	this->m[0][0] = scale_.x;
	this->m[1][1] = scale_.y;
	this->m[2][2] = scale_.z;
	this->m[3][3] = 1.0f;
	return *this;
}

Matrix4x4 Matrix4x4::MakeRotateMatrixX(const Vector3& rot){
	this->m[0][0] = 1.0f;
	this->m[1][1] = cosf(rot.x);
	this->m[1][2] = sinf(rot.x);
	this->m[2][1] = -sinf(rot.x);
	this->m[2][2] = cosf(rot.x);
	this->m[3][3] = 1.0f;

	return *this;
}

Matrix4x4 Matrix4x4::MakeRotateMatrixY(const Vector3& rot){
	this->m[1][1] = 1.0f;
	this->m[0][0] = cosf(rot.y);
	this->m[0][2] = -sinf(rot.y);
	this->m[2][0] = sinf(rot.y);
	this->m[2][2] = cosf(rot.y);
	this->m[3][3] = 1.0f;

	return *this;
}

Matrix4x4 Matrix4x4::MakeRotateMatrixZ(const Vector3& rot){
	this->m[2][2] = 1.0f;
	this->m[0][0] = cosf(rot.z);
	this->m[0][1] = sinf(rot.z);
	this->m[1][0] = -sinf(rot.z);
	this->m[1][1] = cosf(rot.z);
	this->m[3][3] = 1.0f;

	return *this;
}

Matrix4x4 Matrix4x4::MakeRotateMatrix(const Vector3& rot){
	Matrix4x4 rotateMatX, rotateMatY, rotateMatZ;
	rotateMatX.MakeRotateMatrixX(rot);
	rotateMatY.MakeRotateMatrixY(rot);
	rotateMatZ.MakeRotateMatrixZ(rot);

	*this = rotateMatX * rotateMatY * rotateMatZ;

	return *this;
}

Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& translate_){
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;

	m[3][0] = translate_.x;
	m[3][1] = translate_.y;
	m[3][2] = translate_.z;
	return *this;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale_, const Vector3& rot, const Vector3& translate_){

	//スケーリング行列の作成
	Matrix4x4 ScaleMatrix = MakeScaleMatrix(scale_);
	//X,Y,Z軸の回転行列の作成
	Matrix4x4 RotateMatrixX = MakeRotateMatrixX(rot);
	Matrix4x4 RotateMatrixY = MakeRotateMatrixY(rot);
	Matrix4x4 RotateMatrixZ = MakeRotateMatrixZ(rot);
	//回転行列の結合
	Matrix4x4 RotateMatrixXYZ = RotateMatrixX * RotateMatrixY * RotateMatrixZ;
	//平行移動行列の作成
	Matrix4x4 TranslateMatrix = MakeTranslateMatrix(translate_);

	*this = ScaleMatrix * RotateMatrixXYZ * TranslateMatrix;

	return *this;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Matrix4x4& scale, const Matrix4x4& rot, const Matrix4x4& translate){
	*this = scale * rot * translate;
	return *this;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const EulerTransform& transform){
	return MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
}

Matrix4x4 Matrix4x4::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
	m[0][0] = (1.0f / aspectRatio) * (1.0f / tanf(fovY / 2.0f));
	m[1][1] = (1.0f / tanf(fovY / 2.0f));
	m[2][2] = farClip / (farClip - nearClip);
	m[2][3] = 1.0f;
	m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return *this;
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip){
	m[0][0] = 2.0f / (right - left);
	m[1][1] = 2.0f / (top - bottom);
	m[2][2] = 1.0f / (farClip - nearClip);
	m[3][0] = (left + right) / (left - right);
	m[3][1] = (top + bottom) / (bottom - top);
	m[3][2] = nearClip / (nearClip - farClip);
	m[3][3] = 1.0f;

	return *this;
}

Matrix4x4 Matrix4x4::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth){
	m[0][0] = width / 2.0f;
	m[1][1] = -(height / 2.0f);
	m[2][2] = maxDepth - minDepth;
	m[3][0] = left + (width / 2.0f);
	m[3][1] = top + (height / 2.0f);
	m[3][2] = minDepth;
	m[3][3] = 1.0f;

	return *this;
}

Matrix4x4 Matrix4x4::MakeRotateAxisAngle(const Vector3& axis, float angle){
	m[0][0] = (axis.x * axis.x) * (1 - std::cosf(angle)) + std::cosf(angle);
	m[0][1] = (axis.x * axis.y) * (1 - std::cosf(angle)) + (axis.z * std::sinf(angle));
	m[0][2] = (axis.x * axis.z) * (1 - std::cosf(angle)) - (axis.y * std::sinf(angle));
	m[1][0] = (axis.x * axis.y) * (1 - std::cosf(angle)) - (axis.z * std::sinf(angle));
	m[1][1] = (axis.y * axis.y) * (1 - std::cosf(angle)) + std::cosf(angle);
	m[1][2] = (axis.y * axis.z) * (1 - std::cosf(angle)) + (axis.x * std::sinf(angle));
	m[2][0] = (axis.x * axis.z) * (1 - std::cosf(angle)) + (axis.y * std::sinf(angle));
	m[2][1] = (axis.y * axis.z) * (1 - std::cosf(angle)) - (axis.x * std::sinf(angle));
	m[2][2] = (axis.z * axis.z) * (1 - std::cosf(angle)) + std::cosf(angle);
	m[3][3] = 1.0f;
	return *this;
}

Matrix4x4 Matrix4x4::DirectionToDirection(const Vector3& from, const Vector3& to){
	Identity();

	Vector3 axis{};

	Vector3 fn = Vector3::Normalize(from);
	Vector3 tn = Vector3::Normalize(to);

	if (fn == -tn) {
		/*if (fn.x != 0 || fn.z != 0) {
			axis = { 0.0f,fn.z,-fn.x };
		}
		else {
			axis = Vector3::Normalize(Vector3::Cross(fn, tn));
		}*/

		// Y軸と平行か確認
		if (fabs(fn.y) < 0.99f) {  // Y軸に平行でなければY軸を選択
			axis = { 0.0f, 1.0f, 0.0f };
		}
		else {  // Y軸に平行ならZ軸を選択
			axis = { 0.0f, 0.0f, 1.0f };
		}

		// 180°回転行列を生成
		float x = axis.x;
		float y = axis.y;
		float z = axis.z;

		// 180度回転のクォータニオンから回転行列を構築
		m[0][0] = -1 + 2 * (x * x);
		m[0][1] = 2 * x * y;
		m[0][2] = 2 * x * z;

		m[1][0] = 2 * y * x;
		m[1][1] = -1 + 2 * (y * y);
		m[1][2] = 2 * y * z;

		m[2][0] = 2 * z * x;
		m[2][1] = 2 * z * y;
		m[2][2] = -1 + 2 * (z * z);

		return *this;
	}
	else if (fn == tn) {
		return Identity();
	}
	else {
		axis = Vector3::Normalize(Vector3::Cross(fn, tn));
	}


	float cos = Vector3::Dot(fn, tn);
	float sin = Vector3::Length(Vector3::Cross(fn, tn));


	m[0][0] = std::powf(axis.x, 2) * (1.0f - cos) + cos;
	if (std::isnan(m[0][0])) {
		return Identity();
	}
	m[0][1] = axis.x * axis.y * (1.0f - cos) + axis.z * sin;
	m[0][2] = axis.x * axis.z * (1.0f - cos) - axis.y * sin;

	m[1][0] = axis.x * axis.y * (1.0f - cos) - axis.z * sin;
	m[1][1] = std::powf(axis.y, 2) * (1.0f - cos) + cos;
	m[1][2] = axis.y * axis.z * (1.0f - cos) + axis.x * sin;

	m[2][0] = axis.x * axis.z * (1.0f - cos) + axis.y * sin;
	m[2][1] = axis.y * axis.z * (1.0f - cos) - axis.x * sin;
	m[2][2] = std::powf(axis.z, 2) * (1.0f - cos) + cos;

	return *this;
}

Vector3 Matrix4x4::TransformVec(const Vector3& v){
	Vector3 result{
			v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + 1.0f * m[3][0],
			v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + 1.0f * m[3][1],
			v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + 1.0f * m[3][2]
	};
	float w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + 1.0f * m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Vector3 Matrix4x4::TransformNormal(const Vector3& v){
	Vector3 result{
				v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0],
				v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1],
				v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2]
	};

	return result;
}

float Matrix4x4::RotateAngleXFromMatrix() const{
	float angle = std::asin(m[1][1]);
	if (m[1][0] < 0) {
		angle = -angle;  // acosの結果だけでは回転の向きがわからないので符号を調整
	}
	return angle;
}

float Matrix4x4::RotateAngleYFromMatrix() const{
	float angle = std::acos(m[0][0]);
	if (m[2][0] < 0) {
		angle = -angle;  // acosの結果だけでは回転の向きがわからないので符号を調整
	}
	return angle;
}

float Matrix4x4::RotateAngleZFromMatrix() const{
	float angle = std::acos(m[0][0]);
	if (m[0][2] < 0) {
		angle = -angle;  // acosの結果だけでは回転の向きがわからないので符号を調整
	}
	return angle;
}

Vector3 Matrix4x4::RotateAngleFromMatrix() const{

	Vector3 angle{};

	angle.x = RotateAngleXFromMatrix();
	angle.y = RotateAngleYFromMatrix();
	angle.z = RotateAngleZFromMatrix();

	return angle;
	
}

float Matrix4x4::CalculateLengthWithSign(const std::array<float, 3>& v){
	float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	float sign = (v[0] + v[1] + v[2] >= 0.0000000f) ? 1.0f : -1.0f; // 任意の基準で符号を決定
	return length * sign;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& mat) const{
	Matrix4x4 result{};
	for (int y = 0; y < 4; y++){
		for (int x = 0; x < 4; x++){
			result.m[y][x] = this->m[y][x] + mat.m[y][x];
		}
	}

	return result;
}
Matrix4x4 Matrix4x4::operator+=(const Matrix4x4& mat){
	*this = *this + mat;
	return *this;
}
//	減算
Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const{
	Matrix4x4 result;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			result.m[y][x] = this->m[y][x] - mat.m[y][x];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::operator-=(const Matrix4x4& mat)
{
	*this = *this - mat;
	return *this;
}

//	行列の積
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const
{
	Matrix4x4 result;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			for (int i = 0; i < 4; i++) {
				result.m[y][x] += this->m[y][i] * mat.m[i][x];
			}
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::operator*=(const Matrix4x4& mat){
	*this = *this * mat;
	return *this;
}




#include "Quaternion.h"

Quaternion::Quaternion(){
	quaternion_ = { 0.0f,0.0f,0.0f,0.0f };
	vector_ = { 0.0f,0.0f,0.0f };
	w = 0.0f;
}

Quaternion::~Quaternion(){


}

Quaternion* Quaternion::GetInstance(){
	static Quaternion instance;
	return &instance;
}

Quaternion Quaternion::Add(const Quaternion& q0, const Quaternion& q1){
	Quaternion result;
	result.vector_ = Vector3(q0.quaternion_.x, q0.quaternion_.y, q0.quaternion_.z) + Vector3(q1.quaternion_.x, q1.quaternion_.y, q1.quaternion_.z);
	result.w = q0.quaternion_.w + q1.quaternion_.w;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}

Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs){
	Quaternion result;
	Vector3 lhsVec{ lhs.quaternion_.x,lhs.quaternion_.y ,lhs.quaternion_.z };
	Vector3 rhsVec{ rhs.quaternion_.x,rhs.quaternion_.y ,rhs.quaternion_.z };

	result.vector_ = Vector3::Cross(lhsVec, rhsVec) + (lhsVec * rhs.quaternion_.w) + (rhsVec * lhs.quaternion_.w);
	result.w = (lhs.quaternion_.w * rhs.quaternion_.w) - (Vector3::Dot(lhsVec, rhsVec));
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}

Quaternion Quaternion::Multiply(const Quaternion& quaternion, const float n){
	Quaternion result{};
	result.vector_ = Vector3(quaternion.quaternion_.x, quaternion.quaternion_.y, quaternion.quaternion_.z) * n;
	result.w = quaternion.quaternion_.w * n;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}

Quaternion Quaternion::IdentityQuaternion(){
	Quaternion result;
	result.vector_ = { 0.0f,0.0f,0.0f };
	result.w = 1.0f;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}

Quaternion Quaternion::Conjugate(const Quaternion& quaternion){
	Quaternion result;
	Vector3 quater{ quaternion.quaternion_.x,quaternion.quaternion_.y ,quaternion.quaternion_.z };

	result.vector_ = quater * -1.0f;
	result.w = quaternion.quaternion_.w;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;

}

float Quaternion::Norm(const Quaternion& quaternion){
	float result = 0.0f;
	Vector3 quater{ quaternion.quaternion_.x,quaternion.quaternion_.y ,quaternion.quaternion_.z };

	result = sqrtf(powf(quater.x, 2.0f) + powf(quater.y, 2.0f) +
		powf(quater.z, 2.0f) + powf(quaternion.quaternion_.w, 2.0f));
	return result;
}

float Quaternion::Dot(const Quaternion& q0, const Quaternion& q1){
	float result = 0;
	result = q0.quaternion_.x * q1.quaternion_.x +
		q0.quaternion_.y * q1.quaternion_.y +
		q0.quaternion_.z * q1.quaternion_.z +
		q0.quaternion_.w * q1.quaternion_.w;
	return result;
}

Quaternion Quaternion::Reverse(const Quaternion& quaternion){
	Quaternion result{};
	result.vector_.x = -quaternion.quaternion_.x;
	result.vector_.y = -quaternion.quaternion_.y;
	result.vector_.z = -quaternion.quaternion_.z;
	result.w = -quaternion.quaternion_.w;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;	
}

Quaternion Quaternion::Normalize(const Quaternion& quaternion){
	Quaternion result;
	Vector3 quater{ quaternion.quaternion_.x,quaternion.quaternion_.y ,quaternion.quaternion_.z };
	float norm = Norm(quaternion);
	if (norm!=0.0f){
		result.vector_.x = (quater.x / norm);
		result.vector_.y = (quater.y / norm);
		result.vector_.z = (quater.z / norm);
		result.w = (quaternion.quaternion_.w / norm);
	}
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };

	return result;
}

Quaternion Quaternion::Inverse(const Quaternion& quaternion){
	Quaternion result;
	
	result.vector_.x = Conjugate(quaternion).vector_.x / powf(Norm(quaternion), 2.0f);
	result.vector_.y = Conjugate(quaternion).vector_.y / powf(Norm(quaternion), 2.0f);
	result.vector_.z = Conjugate(quaternion).vector_.z / powf(Norm(quaternion), 2.0f);
	result.w = Conjugate(quaternion).w / powf(Norm(quaternion), 2.0f);
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& vector, const float angle){
	Quaternion result;
	result.vector_ = Vector3::Normalize(vector) * std::sinf(angle / 2.0f);
	result.w = std::cosf(angle / 2.0f);
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
}




Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion){
	Vector3 result{};
	Quaternion quaternionNum{};
	Quaternion quaternionVector{};
	quaternionVector.quaternion_ = { vector.x,vector.y, vector.z, 0 };

	quaternionNum = Multiply(Multiply(quaternion, quaternionVector), Conjugate(quaternion));

	result = { quaternionNum.quaternion_.x,quaternionNum.quaternion_.y, quaternionNum.quaternion_.z };
	return result;
}

Matrix4x4 Quaternion::MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result{};
	result.m[0][0] = std::powf(quaternion.quaternion_.w, 2.0f) + std::powf(quaternion.quaternion_.x, 2.0f)
		- std::powf(quaternion.quaternion_.y, 2.0f) - std::powf(quaternion.quaternion_.z, 2.0f);
	result.m[0][1] = 2.0f * (quaternion.quaternion_.x * quaternion.quaternion_.y + quaternion.quaternion_.w * quaternion.quaternion_.z);
	result.m[0][2] = 2.0f * (quaternion.quaternion_.x * quaternion.quaternion_.z - quaternion.quaternion_.w * quaternion.quaternion_.y);

	result.m[1][0] = 2.0f * (quaternion.quaternion_.x * quaternion.quaternion_.y - quaternion.quaternion_.w * quaternion.quaternion_.z);
	result.m[1][1] = std::powf(quaternion.quaternion_.w, 2.0f) - std::powf(quaternion.quaternion_.x, 2.0f)
		+ std::powf(quaternion.quaternion_.y, 2.0f) - std::powf(quaternion.quaternion_.z, 2.0f);
	result.m[1][2] = 2.0f * (quaternion.quaternion_.y * quaternion.quaternion_.z + quaternion.quaternion_.w * quaternion.quaternion_.x);

	result.m[2][0] = 2.0f * (quaternion.quaternion_.x * quaternion.quaternion_.z + quaternion.quaternion_.w * quaternion.quaternion_.y);
	result.m[2][1] = 2.0f * (quaternion.quaternion_.y * quaternion.quaternion_.z - quaternion.quaternion_.w * quaternion.quaternion_.x);
	result.m[2][2] = std::powf(quaternion.quaternion_.w, 2.0f) - std::powf(quaternion.quaternion_.x, 2.0f)
		- std::powf(quaternion.quaternion_.y, 2.0f) + std::powf(quaternion.quaternion_.z, 2.0f);
	result.m[3][3] = 1;
	return result;
}

Quaternion Quaternion::Slerp(Quaternion q0, const Quaternion& q1, float t){
	float dot = Dot(q0, q1);
	if (dot < 0.0f) {
		q0 = Multiply(q0, -1.0f);
		dot = -dot;
	}
	float theata = std::acos(dot);
	float sinTheata = 1.0f / std::sin(theata);

	static constexpr float kEpsilon = 0.0005f;

	Quaternion result;

	// sinθが0.0fになる場合またはそれに近くなる場合
	if (1.0f - kEpsilon <= dot) {
		result = Add(Multiply(q0, (1.0f - t)), Multiply(q1, t));
	}
	else {
		result = Add(Multiply(q0, (std::sin(theata * (1.0f - t)) * sinTheata)), Multiply(q1, (std::sin(theata * t) * sinTheata)));
	}

	return result;
}

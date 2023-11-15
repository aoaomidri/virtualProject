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

Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs){
	Quaternion result;
	Vector3 lhsVec{ lhs.quaternion_.x,lhs.quaternion_.y ,lhs.quaternion_.z };
	Vector3 rhsVec{ rhs.quaternion_.x,rhs.quaternion_.y ,rhs.quaternion_.z };

	result.vector_ = Vector3::Cross(lhsVec, rhsVec) + (lhsVec * rhs.quaternion_.w) + (rhsVec * lhs.quaternion_.w);
	result.w = (lhs.quaternion_.w * rhs.quaternion_.w) - (Vector3::Dot(lhsVec, rhsVec));
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

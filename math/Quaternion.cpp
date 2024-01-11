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
	result.vector_ = Vector3::Cross(lhs.vector_, rhs.vector_) + (lhs.vector_ * rhs.w) + (rhs.vector_ * lhs.w);
	result.w = (lhs.w * rhs.w) - (Vector3::Dot(lhs.vector_, rhs.vector_));
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
	result.vector_ = quaternion.vector_ * -1.0f;
	result.w = quaternion.w;
	result.quaternion_ = { result.vector_.x,result.vector_.y, result.vector_.z, result.w };
	return result;
#ifdef _DEBUG
	int ssef = 32;
#endif // _DEBUG

}

float Quaternion::Norm(const Quaternion& quaternion){
	float result = 0.0f;
	result = sqrtf(powf(quaternion.vector_.x, 2.0f) + powf(quaternion.vector_.y, 2.0f) +
		powf(quaternion.vector_.z, 2.0f) + powf(quaternion.w, 2.0f));
	return result;
}

Quaternion Quaternion::Normalize(const Quaternion& quaternion){
	Quaternion result;
	float norm = Norm(quaternion);
	if (norm!=0.0f){
		result.vector_.x = (quaternion.vector_.x / norm);
		result.vector_.y = (quaternion.vector_.y / norm);
		result.vector_.z = (quaternion.vector_.z / norm);
		result.w = (quaternion.w / norm);
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

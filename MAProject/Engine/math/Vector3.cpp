#include "Vector3.h"
#include"Vector4.h"

Vector3 Vector3::SetVector4(const Vector4& num){
	this->x = num.x;
	this->y = num.y;
	this->z = num.z;
	return *this;
	
}

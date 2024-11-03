#include "Vector4.h"
#include "Vector3.h"

Vector4& Vector4::operator=(const Vector3& num){
	this->x = num.x;
	this->y = num.y;
	this->z = num.z;
	this->w = 1.0f;
	return *this;
	
}

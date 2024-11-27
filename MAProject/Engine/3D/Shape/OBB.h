#pragma once
#pragma once
#include "Matrix.h"
#include <stdint.h>
/*OBBの設定*/
struct OBB
{
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix);
Matrix4x4 GetRotate(const OBB& obb);

void CalculateVertices(const OBB& obb, const Matrix4x4& rotateMatrix, const Vector3& center, Vector3 vertices[8]);

void ProjectOntoAxis(const Vector3 vertices[8], const Vector3& axis, float& min, float& max);

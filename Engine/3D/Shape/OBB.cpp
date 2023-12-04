#include "OBB.h"

void Setorientations(OBB& obb, const Matrix4x4& rotateMatrix)
{
	obb.orientations[0].x = rotateMatrix.m[0][0];
	obb.orientations[0].y = rotateMatrix.m[0][1];
	obb.orientations[0].z = rotateMatrix.m[0][2];
	obb.orientations[1].x = rotateMatrix.m[1][0];
	obb.orientations[1].y = rotateMatrix.m[1][1];
	obb.orientations[1].z = rotateMatrix.m[1][2];
	obb.orientations[2].x = rotateMatrix.m[2][0];
	obb.orientations[2].y = rotateMatrix.m[2][1];
	obb.orientations[2].z = rotateMatrix.m[2][2];
}

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result;
	int size = 4;
	for (int row = 0; row < size; row++) {
		for (int column = 0; column < size; column++) {
			if (row == column) {
				result.m[row][column] = 1;
			}
			else {
				result.m[row][column] = 0;
			}
		}
	}
	return result;
}

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix)
{
	obb.orientations[0].x = rotateMatrix.m[0][0];
	obb.orientations[0].y = rotateMatrix.m[0][1];
	obb.orientations[0].z = rotateMatrix.m[0][2];
	obb.orientations[1].x = rotateMatrix.m[1][0];
	obb.orientations[1].y = rotateMatrix.m[1][1];
	obb.orientations[1].z = rotateMatrix.m[1][2];
	obb.orientations[2].x = rotateMatrix.m[2][0];
	obb.orientations[2].y = rotateMatrix.m[2][1];
	obb.orientations[2].z = rotateMatrix.m[2][2];
}

Matrix4x4 GetRotate(const OBB& obb)
{
	static Matrix matrix;
	Matrix4x4 rotateMatrix;
	rotateMatrix = MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.orientations[0].x;
	rotateMatrix.m[0][1] = obb.orientations[0].y;
	rotateMatrix.m[0][2] = obb.orientations[0].z;
	rotateMatrix.m[1][0] = obb.orientations[1].x;
	rotateMatrix.m[1][1] = obb.orientations[1].y;
	rotateMatrix.m[1][2] = obb.orientations[1].z;
	rotateMatrix.m[2][0] = obb.orientations[2].x;
	rotateMatrix.m[2][1] = obb.orientations[2].y;
	rotateMatrix.m[2][2] = obb.orientations[2].z;

	return rotateMatrix;
}
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

void CalculateVertices(const OBB& obb, const Matrix4x4& rotateMatrix, const Vector3& center, Vector3 vertices[8]) {
	// ローカル空間での8つの頂点座標 (±サイズを使用)
	Vector3 localVertices[8] = {
		{ -obb.size.x, -obb.size.y, -obb.size.z }, // 左下奥
		{ +obb.size.x, -obb.size.y, -obb.size.z }, // 右下奥
		{ -obb.size.x, -obb.size.y, +obb.size.z }, // 左下手前
		{ +obb.size.x, -obb.size.y, +obb.size.z }, // 右下手前
		{ -obb.size.x, +obb.size.y, -obb.size.z }, // 左上奥
		{ +obb.size.x, +obb.size.y, -obb.size.z }, // 右上奥
		{ -obb.size.x, +obb.size.y, +obb.size.z }, // 左上手前
		{ +obb.size.x, +obb.size.y, +obb.size.z }  // 右上手前
	};

	// 各頂点を回転し、中心座標を加算してワールド座標系に変換
	for (int i = 0; i < 8; i++) {
		vertices[i] = Matrix::TransformVec(localVertices[i], rotateMatrix) + center;
	}
}

void ProjectOntoAxis(const Vector3 vertices[8], const Vector3& axis, float& min, float& max) {
	// 投影軸を正規化
	Vector3 normalizedAxis = Vector3::Normalize(axis);

	// 最初の頂点を軸に投影し、minとmaxを初期化
	min = max = Vector3::Dot(vertices[0], normalizedAxis);

	// 残りの頂点も同じ軸に投影し、min/maxを更新
	for (int i = 1; i < 8; i++) {
		float projection = Vector3::Dot(vertices[i], normalizedAxis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}


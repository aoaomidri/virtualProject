#pragma once
#include"OBB.h"
#include"ViewingFrustum.h"

inline bool IsCollisionViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {

	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = Matrix::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f }, { viewingFrustum.rotate_ }, { viewingFrustum.translation_ });

	// 向きベクトルnear
	Vector3 directionNear = Vector3::Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = Vector3::Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = Vector3::Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = Vector3::Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
		directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 左上
	nearPlanePoints_[1] = {
		directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 右上
	nearPlanePoints_[2] = {
		directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 左下
	nearPlanePoints_[3] = {
		directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 右下

	farPlanePoints_[0] = {
		directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 左上
	farPlanePoints_[1] = {
		directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 右上
	farPlanePoints_[2] = {
		directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 左下
	farPlanePoints_[3] = {
		directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 右下

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{ 0 };
	// near
	FrustumPoints[0] = nearPlanePoints_[0];
	FrustumPoints[1] = nearPlanePoints_[1];
	FrustumPoints[2] = nearPlanePoints_[2];
	FrustumPoints[3] = nearPlanePoints_[3];
	// far
	FrustumPoints[4] = farPlanePoints_[0];
	FrustumPoints[5] = farPlanePoints_[1];
	FrustumPoints[6] = farPlanePoints_[2];
	FrustumPoints[7] = farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
		{obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0},
		{obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0},
		{obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0},
		{obb.center.x,          obb.center.y,          obb.center.z,          1} };

	obbPoints[0] = obb.size * -1;
	obbPoints[1] = { obb.size.x * -1, obb.size.y * -1, obb.size.z };
	obbPoints[2] = { obb.size.x, obb.size.y * -1, obb.size.z * -1 };
	obbPoints[3] = { obb.size.x, obb.size.y * -1, obb.size.z };
	obbPoints[4] = { obb.size.x * -1, obb.size.y, obb.size.z * -1 };
	obbPoints[5] = { obb.size.x * -1, obb.size.y, obb.size.z };
	obbPoints[6] = { obb.size.x, obb.size.y, obb.size.z * -1 };
	obbPoints[7] = obb.size;

	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = Matrix::TransformNormal(obbPoints[i], worldMatrix);
		obbPoints[i] = obb.center + obbPoints[i];
	}

	/*ステップ3 OBBを視錐台のローカル座標に変換*/
	// 視錐台の逆行列
	Matrix4x4 FrustumInverceMat = Matrix::Inverce(FrustumMatWorld);
	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = Matrix::TransformVec(obbPoints[i], FrustumInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = FrustumPoints[1] - FrustumPoints[0];
	Vector3 v12 = FrustumPoints[2] - FrustumPoints[1];

	// far
	Vector3 v65 = FrustumPoints[5] - FrustumPoints[6];
	Vector3 v54 = FrustumPoints[4] - FrustumPoints[5];

	// left
	Vector3 v02 = FrustumPoints[2] - FrustumPoints[0];
	Vector3 v26 = FrustumPoints[6] - FrustumPoints[2];

	// right
	Vector3 v53 = FrustumPoints[3] - FrustumPoints[5];
	Vector3 v31 = FrustumPoints[1] - FrustumPoints[3];

	// up
	Vector3 v41 = FrustumPoints[1] - FrustumPoints[4];
	Vector3 v10 = FrustumPoints[0] - FrustumPoints[1];

	// down
	Vector3 v23 = FrustumPoints[3] - FrustumPoints[2];
	Vector3 v36 = FrustumPoints[6] - FrustumPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = Vector3::Normalize(Vector3::Cross(v01, v12));
	// far
	normal[1] = Vector3::Normalize(Vector3::Cross(v65, v54));
	// left
	normal[2] = Vector3::Normalize(Vector3::Cross(v02, v26));
	// right
	normal[3] = Vector3::Normalize(Vector3::Cross(v53, v31));
	// up
	normal[4] = Vector3::Normalize(Vector3::Cross(v41, v10));
	// down
	normal[5] = Vector3::Normalize(Vector3::Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[0]);
		distance[1 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[4], normal[1]);
		distance[2 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[2]);
		distance[3 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[1], normal[3]);
		distance[4 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[4]);
		distance[5 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
			distance[1 + i * normalLine] <= CollisionDistance &&
			distance[2 + i * normalLine] <= CollisionDistance &&
			distance[3 + i * normalLine] <= CollisionDistance &&
			distance[4 + i * normalLine] <= CollisionDistance &&
			distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}

inline bool IsCollisionOBB(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = Matrix::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f }, { viewingFrustum.rotate_ }, { viewingFrustum.translation_ });

	// 向きベクトルnear
	Vector3 directionNear = Vector3::Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = Vector3::Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = Vector3::Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = Vector3::Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
		directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 左上
	nearPlanePoints_[1] = {
		directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 右上
	nearPlanePoints_[2] = {
		directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 左下
	nearPlanePoints_[3] = {
		directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 右下

	farPlanePoints_[0] = {
		directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 左上
	farPlanePoints_[1] = {
		directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 右上
	farPlanePoints_[2] = {
		directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 左下
	farPlanePoints_[3] = {
		directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 右下

	for (int i = 0; i < 4; i++) {
		nearPlanePoints_[i] = Matrix::TransformNormal(nearPlanePoints_[i], FrustumMatWorld);
		farPlanePoints_[i] = Matrix::TransformNormal(farPlanePoints_[i], FrustumMatWorld);
	}

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{ 0 };
	// near
	FrustumPoints[0] = viewingFrustum.translation_ + nearPlanePoints_[0];
	FrustumPoints[1] = viewingFrustum.translation_ + nearPlanePoints_[1];
	FrustumPoints[2] = viewingFrustum.translation_ + nearPlanePoints_[2];
	FrustumPoints[3] = viewingFrustum.translation_ + nearPlanePoints_[3];
	// far
	FrustumPoints[4] = viewingFrustum.translation_ + farPlanePoints_[0];
	FrustumPoints[5] = viewingFrustum.translation_ + farPlanePoints_[1];
	FrustumPoints[6] = viewingFrustum.translation_ + farPlanePoints_[2];
	FrustumPoints[7] = viewingFrustum.translation_ + farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
		{obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0},
		{obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0},
		{obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0},
		{obb.center.x,          obb.center.y,          obb.center.z,          1} };

	// 手前
	obbPoints[0] = { obb.size.x * -1, obb.size.y, obb.size.z * -1 }; // 左上
	obbPoints[1] = { obb.size.x, obb.size.y, obb.size.z * -1 };      // 右上
	obbPoints[2] = obb.size * -1;                                  // 左下
	obbPoints[3] = { obb.size.x, obb.size.y * -1, obb.size.z * -1 }; // 右下
	// 奥
	obbPoints[4] = { obb.size.x * -1, obb.size.y, obb.size.z };      // 左上
	obbPoints[5] = obb.size;                                       // 右上
	obbPoints[6] = { obb.size.x * -1, obb.size.y * -1, obb.size.z }; // 左下
	obbPoints[7] = { obb.size.x, obb.size.y * -1, obb.size.z };      // 右下

	/*ステップ3 視錐台をOBBのローカル座標に変換*/
	// OBBの逆行列
	Matrix4x4 OBBInverceMat = Matrix::Inverce(worldMatrix);
	for (int i = 0; i < OBBVertex; i++) {
		FrustumPoints[i] = Matrix::TransformVec(FrustumPoints[i], OBBInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = obbPoints[1] - obbPoints[0];
	Vector3 v12 = obbPoints[2] - obbPoints[1];

	// far
	Vector3 v65 = obbPoints[5] - obbPoints[6];
	Vector3 v54 = obbPoints[4] - obbPoints[5];

	// left
	Vector3 v02 = obbPoints[2] - obbPoints[0];
	Vector3 v26 = obbPoints[6] - obbPoints[2];

	// right
	Vector3 v53 = obbPoints[3] - obbPoints[5];
	Vector3 v31 = obbPoints[1] - obbPoints[3];

	// up
	Vector3 v41 = obbPoints[1] - obbPoints[4];
	Vector3 v10 = obbPoints[0] - obbPoints[1];

	// down
	Vector3 v23 = obbPoints[3] - obbPoints[2];
	Vector3 v36 = obbPoints[6] - obbPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = Vector3::Normalize(Vector3::Cross(v01, v12));
	// far
	normal[1] = Vector3::Normalize(Vector3::Cross(v65, v54));
	// left
	normal[2] = Vector3::Normalize(Vector3::Cross(v02, v26));
	// right
	normal[3] = Vector3::Normalize(Vector3::Cross(v53, v31));
	// up
	normal[4] = Vector3::Normalize(Vector3::Cross(v41, v10));
	// down
	normal[5] = Vector3::Normalize(Vector3::Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[0]);
		distance[1 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[4], normal[1]);
		distance[2 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[2]);
		distance[3 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[1], normal[3]);
		distance[4 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[4]);
		distance[5 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
			distance[1 + i * normalLine] <= CollisionDistance &&
			distance[2 + i * normalLine] <= CollisionDistance &&
			distance[3 + i * normalLine] <= CollisionDistance &&
			distance[4 + i * normalLine] <= CollisionDistance &&
			distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}
 
inline bool IsCollisionOBBViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	if (IsCollisionOBB(obb, viewingFrustum) || IsCollisionViewFrustum(obb, viewingFrustum)) {
		return true;
	}
	else {
		return false;
	}
}

inline bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2) {


	Vector3 vector{};
	Matrix matrix;

	Vector3 separatingAxis[15]{};
	separatingAxis[0] = obb1.orientations[0];
	separatingAxis[1] = obb1.orientations[1];
	separatingAxis[2] = obb1.orientations[2];
	separatingAxis[3] = obb2.orientations[0];
	separatingAxis[4] = obb2.orientations[1];
	separatingAxis[5] = obb2.orientations[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++) {
		for (int index2 = 0; index2 < 3; index2++) {
			separatingAxis[axisNum] = vector.Cross(obb1.orientations[index1], obb2.orientations[index2]);
			axisNum++;
		}
	}

	Vector3 obb1Vertex[8]{};
	// bottom
	obb1Vertex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vertex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vertex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vertex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	// top
	obb1Vertex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vertex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vertex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vertex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vertex[8]{};
	// bottom
	obb2Vertex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vertex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vertex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vertex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	// top
	obb2Vertex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vertex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vertex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vertex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++) {
		obb1Vertex[index] = matrix.TransformVec(obb1Vertex[index], rotateMatrix1);
		obb1Vertex[index] = (obb1Vertex[index] + obb1.center);
		obb2Vertex[index] = matrix.TransformVec(obb2Vertex[index], rotateMatrix2);
		obb2Vertex[index] = (obb2Vertex[index] + obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8]{};
		float projectionPoint2[8]{};
		float min1, max1;
		float min2, max2;
		min1 = vector.Dot(obb1Vertex[0], vector.Normalize(separatingAxis[axisNum]));
		min2 = vector.Dot(obb2Vertex[0], vector.Normalize(separatingAxis[axisNum]));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
				vector.Dot(obb1Vertex[index], vector.Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] =
				vector.Dot(obb2Vertex[index], vector.Normalize(separatingAxis[axisNum]));
			if (index == 0) {
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else {
				min1 = std::min(min1, projectionPoint1[index]);
				min2 = std::min(min2, projectionPoint2[index]);
				max1 = max(max1, projectionPoint1[index]);
				max2 = max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = max(max1, max2) - std::min(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}



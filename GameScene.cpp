#include "GameScene.h"
#include"math/Vector3.h"
#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/uvChecker.png", 0);
	textureManager_->Load("resources/rock.png", 1);
	textureManager_->Load("resources/Floor.png", 2);
	textureManager_->Load("resources/Road.png", 3);
	textureManager_->Load("resources/Sky.png", 4);
	textureManager_->Load("resources/Enemy/EnemyTex.png", 5);
	textureManager_->Load("resources/EnemyParts/EnemyParts.png", 6);
	textureManager_->Load("resources/Weapon/Sword.png", 7);
}

void GameScene::Initialize(DirectXCommon* dxCommon_){
	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxCommon_->GetDevice(),
		dxCommon_->GetCommandList(), dxCommon_->GetSRVHeap());
	TextureLoad();


	axis = Matrix::GetInstance()->Normalize({ 1.0f,1.0f,1.0f });
	rotateMatrix = Matrix::GetInstance()->MakeRotateAxisAngle(axis, angle);

	from0 = Vector3::Normalize({ 1.0f,0.7f,0.5f });
	to0 = Vector3::Normalize({ -1.0f,-0.7f,-0.5f });
	from1 = Vector3::Normalize({ -0.6f,0.9f,0.2f });
	to1 = Vector3::Normalize({ 0.4f,0.7f,-0.5f });

	rotateMatrix0 = Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize({ 1.0f,0.0f,0.0f }), Vector3::Normalize({ -1.0f,0.0f,0.0f }));
	rotateMatrix1 = Matrix::GetInstance()->DirectionToDirection(from0, to0);
	rotateMatrix2 = Matrix::GetInstance()->DirectionToDirection(from1, to1);

	q1.quaternion_ = { 2.0f,3.0f,4.0f,1.0f };
	q2.quaternion_ = { 1.0f,3.0f,5.0f,2.0f };

	identity = Quaternion::GetInstance()->IdentityQuaternion();
	conj = Quaternion::GetInstance()->Conjugate(q1);
	inv = Quaternion::GetInstance()->Inverse(q1);
	normal = Quaternion::GetInstance()->Normalize(q1);
	mul1 = Quaternion::GetInstance()->Multiply(q1, q2);
	mul2 = Quaternion::GetInstance()->Multiply(q2, q1);
	norm = Quaternion::GetInstance()->Norm(q1);
}

void GameScene::Update(Input* input_){
	DrawImgui();
}

void GameScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDraw3D();
	/*ここから下に描画処理を書き込む*/



	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::DrawImgui(){
	ImGui::Begin("Quaternion");
	ImGui::DragFloat4("Identity", &identity.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat4("Conjugate", &conj.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat4("Inverse", &inv.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat4("Normalize", &normal.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat4("Multiply(q1,q2)", &mul1.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat4("Multiply(q2,q1)", &mul2.quaternion_.x, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Norm", &norm, 0.01f, -100.0f, 100.0f, "%.2f");
	ImGui::End();

	ImGui::Begin("ある方向からある方向へ向ける回転行列");
	if (ImGui::TreeNode("一個目")) {
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix0.m[i], 0.001f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("二個目")) {
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix1.m[i], 0.001f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("三個目")) {
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix2.m[i], 0.001f);
		}
		ImGui::TreePop();
	}


	ImGui::End();
	
}

bool GameScene::IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2){
	static Vector3 vector;
	static Matrix matrix;

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

	Vector3 obb1Vetyex[8]{};
	// bottom
	obb1Vetyex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vetyex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	// top
	obb1Vetyex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vetyex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vetyex[8]{};
	// bottom
	obb2Vetyex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vetyex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	// top
	obb2Vetyex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vetyex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++) {
		obb1Vetyex[index] = matrix.Transform(obb1Vetyex[index], rotateMatrix1);
		obb1Vetyex[index] = (obb1Vetyex[index] + obb1.center);
		obb2Vetyex[index] = matrix.Transform(obb2Vetyex[index], rotateMatrix2);
		obb2Vetyex[index] = (obb2Vetyex[index] + obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8];
		float projectionPoint2[8];
		float min1, max1;
		float min2, max2;
		min1 = vector.Dot(obb1Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		min2 = vector.Dot(obb2Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
				vector.Dot(obb1Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] =
				vector.Dot(obb2Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			if (index == 0) {
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else {
				min1 = min(min1, projectionPoint1[index]);
				min2 = min(min2, projectionPoint2[index]);
				max1 = max(max1, projectionPoint1[index]);
				max2 = max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = max(max1, max2) - min(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}

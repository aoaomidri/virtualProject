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

	rotation0_ = Quaternion::GetInstance()->MakeRotateAxisAngleQuaternion(Vector3::Normalize({ 0.71f,0.71f,0.0f }), 0.3f);
	rotation1_.quaternion_ = { -rotation0_.quaternion_.x,-rotation0_.quaternion_.y ,-rotation0_.quaternion_.z ,-rotation0_.quaternion_.w };

	rotation0_ = Quaternion::GetInstance()->Normalize(rotation0_);
	rotation1_ = Quaternion::GetInstance()->Normalize(rotation1_);

	interpolate_[0] = Quaternion::GetInstance()->Slerp(rotation0_, rotation1_, 0.0f);
	interpolate_[1] = Quaternion::GetInstance()->Slerp(rotation0_, rotation1_, 0.3f);
	interpolate_[2] = Quaternion::GetInstance()->Slerp(rotation0_, rotation1_, 0.5f);
	interpolate_[3] = Quaternion::GetInstance()->Slerp(rotation0_, rotation1_, 0.7f);
	interpolate_[4] = Quaternion::GetInstance()->Slerp(rotation0_, rotation1_, 1.0f);
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
	ImGui::DragFloat4("interpolate0", &interpolate_[0].quaternion_.x, 0.01f, -100.0f, 100.0f, "%.5f");
	ImGui::DragFloat4("interpolate1", &interpolate_[1].quaternion_.x, 0.01f, -100.0f, 100.0f, "%.5f");
	ImGui::DragFloat4("interpolate2", &interpolate_[2].quaternion_.x, 0.01f, -100.0f, 100.0f, "%.5f");
	ImGui::DragFloat4("interpolate3", &interpolate_[3].quaternion_.x, 0.01f, -100.0f, 100.0f, "%.5f");
	ImGui::DragFloat4("interpolate4", &interpolate_[4].quaternion_.x, 0.01f, -100.0f, 100.0f, "%.5f");
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
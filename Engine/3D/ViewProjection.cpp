#include "ViewProjection.h"

void ViewProjection::UpdateMatrix(){
	Matrix4x4 cameraMatrix = Matrix::GetInstance()->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_);
	matView_ = Matrix::GetInstance()->Inverce(cameraMatrix);

	matProjection_ = Matrix::GetInstance()->MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

	matViewProjection_ = Matrix::GetInstance()->Multiply(matView_, matProjection_);
}

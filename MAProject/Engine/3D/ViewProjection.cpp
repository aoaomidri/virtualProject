#include "ViewProjection.h"

void ViewProjection::UpdateMatrix(){
	cameraMatrix_ = Matrix::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_);
	matView_ = Matrix::Inverce(cameraMatrix_);

	matProjection_ = Matrix::MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

	matViewProjection_ = Matrix::Multiply(matView_, matProjection_);
}

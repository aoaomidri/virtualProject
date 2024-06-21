#include "PointLight.h"

void PointLight::Initialize() {
	lightData_ = std::make_unique<PointLightData>();
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->position = { 0.0f,10.0f,0.0f };
	lightData_->intensity = { 1.0f };
	lightData_->radius = 10.0f;
	lightData_->decay = 1.0f;
}

#include "DirectionalLight.h"

void DirectionalLight::Initialize() {
	lightData_ = std::make_unique<DirectionalLightData>();
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->direction = { 0.0f,-1.0f,0.0f };
	lightData_->intensity = { 1.0f};
}

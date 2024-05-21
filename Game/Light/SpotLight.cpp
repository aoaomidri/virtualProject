#include "SpotLight.h"

void SpotLight::Initialize() {
	lightData_ = std::make_unique<SpotLightData>();
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->direction = { 0.0f,-1.0f,0.0f };
	lightData_->intensity = { 1.0f};
}

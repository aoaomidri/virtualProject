#include "StageObject.h"

void StageObject::Initialize(){}

void StageObject::Draw(const ViewProjection& viewProjection){
	const auto objs = LevelLoader::GetInstance()->GetStationaryObject();

	for (size_t i = 0; i < objs.size(); i++){
		objs[i]->SetIsLighting(true);
		objs[i]->SetShininess(2.0f);

		objs[i]->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());
		objs[i]->Update(viewProjection);
		objs[i]->Draw();
	}
}

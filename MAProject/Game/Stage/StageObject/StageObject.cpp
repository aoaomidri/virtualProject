#include "StageObject.h"

void StageObject::Initialize(){}

void StageObject::Draw(const ViewProjection& viewProjection) const{
	const auto objs = LevelLoader::GetInstance()->GetStationaryObject();
	//全オブジェを更新
	for (size_t i = 0; i < objs.size(); i++){
		objs[i]->SetIsLighting(true);
		objs[i]->SetShininess(shininess_);

		objs[i]->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());
		objs[i]->Update(viewProjection);
		objs[i]->Draw();
	}
}

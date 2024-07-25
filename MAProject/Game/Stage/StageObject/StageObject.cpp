#include "StageObject.h"

void StageObject::Initialize(){}

void StageObject::Draw(const ViewProjection& viewProjection){
	const auto objs = LevelLoader::GetInstance()->GetStationaryObject();

	for (size_t i = 0; i < objs.size(); i++){
		objs[i]->Update(viewProjection);
		objs[i]->Draw();
	}
}

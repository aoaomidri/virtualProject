#pragma once
#include"Matrix4x4.h"
/*スポットライトの設定*/
class SpotLight{
public:
	struct SpotLightData {
		Vector4 color;		//ライトの色
		Vector3 direction;	//ライトの向き
		float intensity;	//輝度
	};

public:

	const SpotLightData* GetLightData()const { return lightData_.get(); }

	void SetLightData(SpotLightData data){
		lightData_->color = data.color;
		lightData_->direction = data.direction;
		lightData_->intensity = data.intensity;
	}

public:
	void Initialize();

	

private:
	std::unique_ptr<SpotLightData> lightData_;


};


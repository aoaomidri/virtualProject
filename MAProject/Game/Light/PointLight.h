#pragma once
#include"Matrix4x4.h"
/*点光源の設定*/
class PointLight{
public:
	struct PointLightData {
		Vector4 color;		//ライトの色
		Vector3 position;	//ライトの位置
		float intensity;	//輝度
		float radius;		//ライトの届く最大距離
		float decay;		//減衰率
		float padding[2];
	};

public:

	const PointLightData* GetLightData()const { return lightData_.get(); }

	void SetLightData(PointLightData data){
		lightData_->color = data.color;
		lightData_->position = data.position;
		lightData_->intensity = data.intensity;
		lightData_->radius = data.radius;
		lightData_->decay = data.decay;
	}

public:

	void Initialize();
	

private:
	std::unique_ptr<PointLightData> lightData_;


};


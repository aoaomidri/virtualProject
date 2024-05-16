#pragma once
#include"Matrix4x4.h"

class DirectionalLight{
public:
	struct DirectionalLightData {
		Vector4 color;		//ライトの色
		Vector3 direction;	//ライトの向き
		float intensity;	//輝度
	};

public:
	DirectionalLight() = default;
	~DirectionalLight() = default;
	DirectionalLight(const DirectionalLight& TextureManager) = delete;
	DirectionalLight& operator=(const DirectionalLight&) = delete;

	static DirectionalLight* GetInstance() {
		static DirectionalLight instance;
		return &instance;
	}

	const DirectionalLightData* GetLightData()const { return lightData_.get(); }

	void SetLightData(DirectionalLightData data){
		lightData_->color = data.color;
		lightData_->direction = data.direction;
		lightData_->intensity = data.intensity;
	}

public:
	void Initialize();

	

private:
	std::unique_ptr<DirectionalLightData> lightData_;


};


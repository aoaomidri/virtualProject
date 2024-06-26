#include "Object3d.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};

struct DirectionalLight {
	float32_t4 color;		//ライトの色
	float32_t3 direction;	//ライトの向き
	float intensity;		//輝度
};

struct PointLight {
	float32_t4 color;		//ライトの色
	float32_t3 position;	//ライトの位置
	float intensity;		//輝度
	float radius;			//ライトの届く最大距離
	float decay;			//減衰率
};

struct Camera{
	float32_t3 worldPosition;
};

struct Threshold{
	float32_t threshold;
};

ConstantBuffer<Material> gMaterial : register(b1);

Texture2D<float32_t4> gTexture : register(t0);

Texture2D<float32_t> gMaskTexture : register(t2);

SamplerState gSampler : register(s0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

ConstantBuffer<Camera> gCamera : register(b3);

ConstantBuffer<PointLight> gPointLight : register(b4);

ConstantBuffer<Threshold> gThreshold : register(b5);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord,0.0f,1.0f),gMaterial.uvTransform);

	float32_t4 textureColor = gTexture.Sample(gSampler,transformedUV.xy);

	float32_t3 toEye = normalize(gCamera.worldPosition-input.worldPosition);

	if(gMaterial.enableLighting != 0){
		//ここからディレクショナルライト
		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
		float NDotH = dot(normalize(input.normal),halfVector);
		float specularPow = pow(saturate(NDotH),gMaterial.shininess);//反射強度

		float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		// //拡散反射
		float32_t3 diffuseDirectionalLight = 
		gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;

		// //鏡面反射
		float32_t3 specularDirectionalLight = 
		gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f,1.0f,1.0f);

		// //ここからポイントライト

		float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
		halfVector = normalize(-pointLightDirection + toEye);
		NDotH = dot(normalize(input.normal),halfVector);
		specularPow=pow(saturate(NDotH),gMaterial.shininess);//反射強度

		NdotL = dot(normalize(input.normal),-pointLightDirection);
		cos = pow(NdotL * 0.5f + 0.5f, 2.0f);


		
		//減衰計算
		float32_t distance = length(gPointLight.position - input.worldPosition);//ポイントライトへの距離
		float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay); //指数によるコントロール

		//拡散反射
		float32_t3 diffusePointLight = 
		gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;

		//鏡面反射
		float32_t3 specularPointLight = 
		gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f,1.0f,1.0f) * factor;

		float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
		//maskの値が閾値以下の場合はdiscardして抜く
		if(mask <= gThreshold.threshold){
			discard;
		}

		//Edgeっぽさを算出
		float32_t edge = 1.0f - smoothstep(gThreshold.threshold, gThreshold.threshold + 0.02f,mask);

		output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;

		output.color.rgb += edge * float32_t3(1.0f,0.4f,0.3f);
		
		output.color.a = gMaterial.color.a * textureColor.a;
		
	}else{
		float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
		//maskの値が閾値以下の場合はdiscardして抜く
		if(mask <= gThreshold.threshold){
			discard;
		}

		//Edgeっぽさを算出
		float32_t edge = 1.0f - smoothstep(gThreshold.threshold, gThreshold.threshold + 0.02f,mask);

		output.color = gMaterial.color * textureColor;
		
		output.color.rgb += edge * float32_t3(1.0f,0.4f,0.3f);

		
	}
	if (textureColor.a <= 0.5 || output.color.a ==0.0){
		discard;
	}
	return output;
}
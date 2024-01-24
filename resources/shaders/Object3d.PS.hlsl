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
	float intensity;	//輝度
};

struct Camera{
	float32_t3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b1);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

ConstantBuffer<Camera> gCamera : register(b3);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord,0.0f,1.0f),gMaterial.uvTransform);

	float32_t4 textureColor = gTexture.Sample(gSampler,transformedUV.xy);

	float32_t3 toEye = normalize(gCamera.worldPosition-input.worldPosition);

	if(gMaterial.enableLighting != 0){
		float NdotL = dot(normalize(input.normal),gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		
		// float32_t3 reflectLight = reflect(-gDirectionalLight.direction,normalize(input.normal));
		// float RdotE=dot(reflectLight,toEye);
		float32_t3 halfVector = normalize(gDirectionalLight.direction + toEye);
		float NDotH = dot(normalize(input.normal),halfVector);
		float specularPow=pow(saturate(NDotH),gMaterial.shininess);//反射強度

		//拡散反射
		float32_t3 diffuse = 
		gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;

		//鏡面反射
		float32_t3 specular = 
		gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f,1.0f,1.0f);

		output.color.rgb = diffuse + specular;
		output.color.a = gMaterial.color.a * textureColor.a;
	}else{
		output.color = gMaterial.color * textureColor;
	}
	if (textureColor.a <= 0.5 || output.color.a ==0.0){
		discard;
	}
	return output;
}
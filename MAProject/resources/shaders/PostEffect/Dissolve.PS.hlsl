#include "FullScreen.hlsli"

struct Threshold{
	float32_t threshold;
};

Texture2D<float32_t4> gTexture : register(t0);

Texture2D<float32_t> gMaskTexture : register(t1);

SamplerState gSampler : register(s0);

ConstantBuffer<Threshold> gThreshold : register(b0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
	//maskの値が閾値以下の場合はdiscardして抜く
	if(mask <= gThreshold.threshold){
		discard;
	}

	//Edgeっぽさを算出
	float32_t edge = 1.0f - smoothstep(gThreshold.threshold, gThreshold.threshold + 0.02f,mask);

	output.color = gTexture.Sample(gSampler,input.texcoord);
	
	output.color.rgb += edge * float32_t3(1.0f,0.4f,0.3f);

	output.color.rgb = AdjustHSV(output.color.rgb,gMaterial);
	return output;
}
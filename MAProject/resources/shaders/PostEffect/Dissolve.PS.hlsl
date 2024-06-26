#include "FullScreen.hlsli"

struct Threshold{
	float32_t threshold;
};

Texture2D<float32_t4> gTexture : register(t0);

Texture2D<float32_t> gMaskTexture : register(t1);

SamplerState gSampler : register(s0);

ConstantBuffer<Threshold> gThreshold : register(b0);

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

	output.color = gTexture.Sample(gSampler,input.texcoord);
	
	return output;
}
#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t4 textureColor =  gTexture.Sample(gSampler,input.texcoord);

	output.color.rgb = AdjustHSV(textureColor.rgb, gMaterial);
	output.color.a = textureColor.a;
	
	return output;
}
#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

ConstantBuffer<PostBlend> gBlend : register(b3);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t4 textureColor = gTexture.Sample(gSampler,input.texcoord);
	float32_t value = dot(textureColor.rgb,float32_t3(0.2125f,0.7154f,0.0721f));

	float32_t3 grayColor = float32_t3(value,value,value);

	grayColor = AdjustHSV(grayColor.rgb,gMaterial);
	// アルファ値は元のテクスチャから
	float32_t4 finalGrayColor = float32_t4(grayColor, textureColor.a);

	output.color = lerp(textureColor,finalGrayColor,gBlend.blendFactor);

	return output;
}
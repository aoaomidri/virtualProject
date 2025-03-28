#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	output.color = gTexture.Sample(gSampler,input.texcoord);

	//周囲を0に、中心になるほど明るくなるように計算で調整
	float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
	//correctだけで計算すると中心の最大値が0.0625で暗すぎるのでscaleで調整。この例では16倍にして1にしている
	float vignette = correct.x * correct.y * 16.0f;
	//とりあえず0.8乗でそれっぽくしてみた。
	vignette = saturate(pow(vignette, 0.8f));
	//係数として乗算
	output.color.rgb *= vignette;

	float32_t value = dot(output.color.rgb,float32_t3(0.2125f,0.7154f,0.0721f));
	output.color.rgb = float32_t3(value,value,value);	

	output.color.rgb = AdjustHSV(output.color.rgb,gMaterial);

	return output;
}
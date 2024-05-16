#include "FullScreen.hlsli"

struct Vignetting{
	float32_t scale;
	float32_t pow;
};

ConstantBuffer<Vignetting> gVignetting : register(b1);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	output.color = gTexture.Sample(gSampler,input.texcoord);
	float32_t value = dot(output.color.rgb,float32_t3(0.2125f,0.7154f,0.0721f));
	output.color.rgb = float32_t3(value,value,value);

	//周囲を0に、中心になるほど明るくなるように計算で調整
	float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
	//correctだけで計算すると中心の最大値が0.0625で暗すぎるのでscaleで調整。この例では16倍にして1にしている
	float vignette = correct.x * correct.y * gVignetting.scale;
	//とりあえず0.8乗でそれっぽくしてみた。
	vignette = saturate(pow(vignette, gVignetting.pow));
	//係数として乗算
	output.color.rgb *= vignette;

	return output;
}
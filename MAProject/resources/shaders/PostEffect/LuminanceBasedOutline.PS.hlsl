#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};
static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f}},
	{{-1.0f, 0.0f},{0.0f, 0.0f},{1.0f, 0.0f}},
	{{-1.0f, 1.0f},{0.0f, 1.0f},{1.0f, 1.0f}},
};

static const float32_t kPrewittHorizontalKernel[3][3] = {
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
	{-1.0f / 6.0f, 0.0f, 1.0f / 6.0f},
};

static const float32_t kPrewittVerticalKernel[3][3] = {
	{-1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f},
	{0.0f, 0.0f, 0.0f},
	{1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f},
};

float32_t Luminance(float32_t3 v){
	return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input) {
	uint32_t width, height;//1, uvStepSizeの算出
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	float32_t2 difference = float32_t2(0.0f,0.0f);//縦横それぞれの畳み込みの結果を格納する
	//色を輝度に変換して、畳み込みを行っていく。微分filter用のkernelになっているので、やること自体は今までの畳み込みと同じ
	for(int32_t x = 0; x < 3; ++x) {
		for(int32_t y = 0; y < 3; ++y){
			float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
			float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			float32_t luminance = Luminance(fetchColor);
			difference.x += luminance * kPrewittHorizontalKernel[x][y];
			difference.y += luminance * kPrewittVerticalKernel[x][y];
		}

	}

	float32_t weight = length(difference);
	weight = saturate(weight * 6.0f);

	PixelShaderOutput output;
	output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler,input.texcoord).rgb;

	output.color.rgb = AdjustHSV(output.color.rgb,gMaterial);

	output.color.a = 1.0f;
	
	return output;
}
#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

static const float32_t PI = 3.1415926535f;

float gauss(float x, float y, float sigma){
	float exponent = -(x*x+y*y)*rcp(2.0f*sigma*sigma);
	float denominater = 2.0f * PI * sigma * sigma;
	return exp(exponent) * rcp(denominater);
}

PixelShaderOutput main(VertexShaderOutput input) {
	float32_t weight = 0.0f;
	float32_t kernel5x5[5][5];
	uint32_t width, height;//1, uvStepSizeの算出
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	PixelShaderOutput output;
	output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
	output.color.a = 1.0f;
	for (int32_t x = 0; x < 5; ++x) {
        for (int32_t y = 0; y < 5; ++y) {
			kernel5x5[x][y] = gauss(x-2.0f, y-2.0f, 2.0f);
			weight += kernel5x5[x][y];

            // 現在のテクスチャ座標を算出
            float32_t2 texcoord = input.texcoord + float32_t2(x-2.0f, y-2.0f) * uvStepSize;
            // 色にカーネルの値を掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel5x5[x][y];
        }
    }	
	
	output.color.rgb *= rcp(weight);

	return output;
}

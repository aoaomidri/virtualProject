#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

static const float32_t kKernel9x9[9][9] = {
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
	{1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f, 1.0f / 81.0f},
};

PixelShaderOutput main(VertexShaderOutput input) {
	uint32_t width, height;//1, uvStepSizeの算出
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	PixelShaderOutput output;
	output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
	output.color.a = 1.0f;
	for (int32_t x = -4; x <= 4; ++x) {
        for (int32_t y = -4; y <= 4; ++y) {
            // 現在のテクスチャ座標を算出
            float32_t2 texcoord = input.texcoord + float32_t2(x, y) * uvStepSize;
            // 色にカーネルの値を掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kKernel9x9[x + 4][y + 4];
        }
    }	
	
	return output;
}
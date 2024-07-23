#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	// テクスチャからサンプルを取得して色を取得
    float32_t4 sampledColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 色を反転する
    float32_t4 invertedColor = float32_t4(1.0f, 1.0f, 1.0f, 1.0f) - sampledColor;

    // 出力色を設定
    output.color = invertedColor;

    output.color.rgb = AdjustHSV(output.color.rgb,gMaterial);
	
	return output;
}
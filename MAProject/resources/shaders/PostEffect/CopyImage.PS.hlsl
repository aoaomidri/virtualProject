#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct HSVMaterial {
	float32_t hue;
	float32_t saturation;
	float32_t value;
};

ConstantBuffer<HSVMaterial> gMaterial : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};



float32_t WrapValue(float32_t value ,float32_t minRange, float32_t maxRange){
	float32_t range = maxRange - minRange;
	float32_t modValue = fmod(value - minRange, range);
	if(modValue < 0){
		modValue += range;
	}
	return minRange + modValue;
}

float32_t3 RGBToHSV(float32_t3 rgb){
	float32_t r = rgb.x;
	float32_t g = rgb.y;
	float32_t b = rgb.z;
	//最大値と最小値を決めておく
	float32_t max = r > g ? r : g;
	max = max > b ? max : b;
	float32_t min = r < g ? r : g;
	min = min < b ? min : b;
	//hの値を決める
	float32_t h = max - min;
	if(h > 0.0f){
		if(max == r){
			h = (g - b) / h;
			if(h < 0.0f){
				h += 6.0f;
			}
		}else if(max == g){
			h = 2.0f + (b - r) / h;
		}else{
			h = 4.0f + (r - g) / h;
		}

	}
	h /= 6.0f;
	//sの値を決める
	float32_t s = (max - min);
	if(max != 0.0f)
	s /= max;
	//vの値を決める
	float32_t v = max;

	float32_t3 hsv = float32_t3(h,s,v);
	return hsv;
}

float32_t3 HSVToRGB(float32_t3 hsv){
	// float32_t h = hsv.r;
	// float32_t s = hsv.g;
	// float32_t v = hsv.b;
	// float32_t r = hsv.b;
	// float32_t g = hsv.b;
	// float32_t b = hsv.b;

	// if(s < 0.0f){
	// 	h *= 6.0f;
	// 	final int32_t i =(int32_t) h;
	// 	final float32_t f = h - (float32_t)i;
	// }
	

	// float32_t3 rgb = float32_t3(r,g,b);
	// return rgb;
	float32_t h = hsv.x;
    float32_t s = hsv.y;
    float32_t v = hsv.z;

    float32_t r = 0.0f, g = 0.0f, b = 0.0f;

    int32_t i = (int32_t)floor(h * 6.0f);
    float32_t f = (h * 6.0f) - i;
    float32_t p = v * (1.0f - s);
    float32_t q = v * (1.0f - f * s);
    float32_t t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    return float32_t3(r, g, b);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t4 textureColor =  gTexture.Sample(gSampler,input.texcoord);

	float32_t3 hsv = RGBToHSV(textureColor.rgb);

	//ここで調整
	hsv.x += gMaterial.hue;
	hsv.y += gMaterial.saturation;
	hsv.z += gMaterial.value;

	hsv.x = WrapValue(hsv.x,0.0f,1.0f);
	hsv.y = saturate(hsv.y);
	hsv.z = saturate(hsv.z);


	float32_t3 rgb = HSVToRGB(hsv);

	output.color.rgb = rgb;
	output.color.a = textureColor.a;
	
	return output;
}
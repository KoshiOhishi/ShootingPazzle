#include "ShadowTest.hlsli"

Texture2D<float> tex : register(t2);	//2番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET1
{
	float colortex = tex.Sample(smp, input.uv);
	return float4(colortex, colortex, colortex, 1);
}
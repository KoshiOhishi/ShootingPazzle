#include "OBJShadowTest.hlsli"

float4 main(float4 pos : POSITION, float2 uv : TEXCOORD) : SV_POSITION
{
	return mul(mul(lightcamera, world), pos);
}
#include "InstancingOBJShadowTest.hlsli"

float4 main(VSInput input) : SV_POSITION
{
	uint index = input.instanceID;

	return mul(mul(lightcamera, world[index]), input.pos);
}
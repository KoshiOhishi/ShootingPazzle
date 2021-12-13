#include "OBJ.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//ピクセルシェーダに渡す値
	VSOutput output;
	output.svpos = pos;
	output.worldpos = pos;
	output.normal = normal.xyz;
	output.uv = uv;
	output.mag = mag;

	return output;
}
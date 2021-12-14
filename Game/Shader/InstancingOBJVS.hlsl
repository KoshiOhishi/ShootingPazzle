#include "InstancingOBJ.hlsli"

VSOutput main(VSInput input)
{
	//ピクセルシェーダに渡す値
	VSOutput output;
	
	uint index = input.instanceID;
	
	//法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(data[index].world, float4(input.normal, 0)));
	float4 wpos = mul(data[index].world, input.pos);
	
	output.svpos = mul(mul(viewproj, data[index].world), input.pos);
	output.wpos = wpos;
	output.normal = wnormal;
	output.uv = input.uv;
	output.color = data[index].color;

	return output;
}
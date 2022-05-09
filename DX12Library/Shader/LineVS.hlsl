#include "Line.hlsli"

VSOutput main(VSInput input)
{
	//法線にワールド行列によるスケーリング・回転を適用
	float4 wpos = mul(world, float4(input.pos, 1));

	//ピクセルシェーダに渡す値
	VSOutput output;
	output.pos = wpos;
	output.color = input.color;
	
	return output;
}
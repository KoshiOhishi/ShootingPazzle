#include "OBJ.hlsli"

VSOutput main(VSInput input)
{
	//ピクセルシェーダに渡す値
	VSOutput output;
			
	//ワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(data.world, float4(input.normal, 0)));
	float4 wpos = mul(data.world, input.pos);
	
	//output.svpos = mul(mul(lightcamera, data.world), input.pos);
	output.svpos = mul(mul(viewproj, data.world), input.pos);
	output.worldpos = wpos;
	output.tpos = mul(mul(lightcamera, data.world), input.pos);
	output.normal = wnormal;
	output.uv = input.uv;

	return output;

}
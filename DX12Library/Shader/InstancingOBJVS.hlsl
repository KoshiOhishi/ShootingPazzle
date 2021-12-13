#include "InstancingOBJ.hlsli"

VSOutput main(VSInput input)
{
	//ジオメトリシェーダに渡す値
	VSOutput output;
	
	uint index = input.instanceID;
	
	output.svpos = input.pos;
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}
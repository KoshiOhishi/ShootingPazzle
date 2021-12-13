#include "OBJ.hlsli"

VSOutput main(VSInput input)
{
	//ジオメトリシェーダに渡す値
	VSOutput output;
		
	output.svpos = input.pos;
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}
#include "InstancingOBJ.hlsli"

float GetRand(float2 texCoord, int Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

[maxvertexcount(3)]
void main(
	triangle VSOutput input[3] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos = input[i].svpos;
		
		////法線にワールド行列によるスケーリング・回転を適用
		//float4 wnormal = normalize(mul(data[instanceID].world, float4(input[i].normal, 0)));
		//float4 wpos = mul(data[instanceID].world, element.svpos);
		
		//element.svpos = mul(mul(viewproj, data[instanceID].world), element.svpos);
		//element.normal = wnormal;
		//element.worldpos = wpos;
		//element.uv = input[i].uv;
		//element.color = data[instanceID].color;
		
		element.svpos = input[i].svpos;
		element.normal = input[i].normal;
		element.worldpos = input[i].wpos;
		element.uv = input[i].uv;
		element.color = input[i].color;
		
		output.Append(element);
	}
	
}
#include "OBJ.hlsli"

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
		element.svpos = input[i].svpos + float4(input[i].normal * mag, 0);
		element.normal = input[i].normal;
		element.worldpos = input[i].worldpos;
		element.uv = input[i].uv;
		
		//法線にワールド行列によるスケーリング・回転を適用
		float4 wnormal = normalize(mul(world, float4(input[i].normal, 0)));
		float4 wpos = mul(world, element.svpos);
		
		element.svpos = mul(mul(viewproj, world), element.svpos);
		element.normal = wnormal;
		element.worldpos = wpos;
		
		output.Append(element);
	}
	
}
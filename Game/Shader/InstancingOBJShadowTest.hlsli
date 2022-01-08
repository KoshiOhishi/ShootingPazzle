cbuffer cbuff0 : register(b0)
{
	matrix world[512];
}

cbuffer cbuff1 : register(b1)
{
	matrix lightcamera;
}

struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD; //uv’l
	uint instanceID : SV_InstanceID;
};
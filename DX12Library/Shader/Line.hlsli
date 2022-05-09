cbuffer cbuff0 : register(b0)
{
	//matrix mat; // ３Ｄ変換行列
	float4 color;
	matrix world;		//ワールド行列
};

struct VSInput
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};
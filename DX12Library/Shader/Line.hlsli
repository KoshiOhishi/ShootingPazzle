cbuffer cbuff0 : register(b0)
{
	//matrix mat; // �R�c�ϊ��s��
	float4 color;
	matrix world;		//���[���h�s��
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
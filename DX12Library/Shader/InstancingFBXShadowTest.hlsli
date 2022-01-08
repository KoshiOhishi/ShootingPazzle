cbuffer cbuff0 : register(b0)
{
	matrix world[512];
}

cbuffer cbuff1 : register(b1)
{
	matrix lightcamera;
}

//ボーンの最大数
static const int MAX_BONES = 32;

cbuffer skinning : register(b2) //ボーンのスキニング行列が入る
{
	matrix matSkinning[MAX_BONES];
}

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos : POSITION; //位置
	float2 uv : TEXCOORD; //uv値
	uint4 boneIndices : BONEINDICES; //ボーンの番号
	float4 boneWeights : BONEWEIGHTS; //ボーンのスキンウェイト
	uint instanceID : SV_InstanceID;
};
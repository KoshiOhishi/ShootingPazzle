cbuffer cbuff0 : register(b0)
{
	matrix lightcamera;
	matrix world;
}

//ボーンの最大数
static const int MAX_BONES = 32;

cbuffer skinning : register(b1) //ボーンのスキニング行列が入る
{
	matrix matSkinning[MAX_BONES];
}

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos : POSITION; //位置
	float2 uv : TEXCOORD;
	uint4 boneIndices : BONEINDICES; //ボーンの番号
	float4 boneWeights : BONEWEIGHTS; //ボーンのスキンウェイト
};
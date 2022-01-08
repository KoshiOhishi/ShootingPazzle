#include "InstancingFBXShadowTest.hlsli"

//スキニング後の頂点・法線が入る
struct SkinOutput
{
	float4 pos;
	float3 normal;
};

//スキニング計算
SkinOutput ComputeSkin(VSInput input)
{
	//ゼロクリア
	SkinOutput output = (SkinOutput) 0;

	uint iBone; //計算するボーン番号
	float weight; //ボーンウェイト（重み）
	matrix m; //スキニング行列

	//ボーン0
	iBone = input.boneIndices.x;
	weight = input.boneWeights.x;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//ボーン1
	iBone = input.boneIndices.y;
	weight = input.boneWeights.y;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//ボーン2
	iBone = input.boneIndices.z;
	weight = input.boneWeights.z;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//ボーン3
	iBone = input.boneIndices.w;
	weight = input.boneWeights.w;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	return output;
}

float4 main(VSInput input) : SV_POSITION
{ 
	uint index = input.instanceID;

	//スキニング計算
	SkinOutput skinned = ComputeSkin(input);
	return mul(mul(lightcamera, world[index]), skinned.pos);
}
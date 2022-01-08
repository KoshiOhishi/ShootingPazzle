#include "InstancingFBXShadowTest.hlsli"

//�X�L�j���O��̒��_�E�@��������
struct SkinOutput
{
	float4 pos;
	float3 normal;
};

//�X�L�j���O�v�Z
SkinOutput ComputeSkin(VSInput input)
{
	//�[���N���A
	SkinOutput output = (SkinOutput) 0;

	uint iBone; //�v�Z����{�[���ԍ�
	float weight; //�{�[���E�F�C�g�i�d�݁j
	matrix m; //�X�L�j���O�s��

	//�{�[��0
	iBone = input.boneIndices.x;
	weight = input.boneWeights.x;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//�{�[��1
	iBone = input.boneIndices.y;
	weight = input.boneWeights.y;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//�{�[��2
	iBone = input.boneIndices.z;
	weight = input.boneWeights.z;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	//�{�[��3
	iBone = input.boneIndices.w;
	weight = input.boneWeights.w;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);

	return output;
}

float4 main(VSInput input) : SV_POSITION
{ 
	uint index = input.instanceID;

	//�X�L�j���O�v�Z
	SkinOutput skinned = ComputeSkin(input);
	return mul(mul(lightcamera, world[index]), skinned.pos);
}
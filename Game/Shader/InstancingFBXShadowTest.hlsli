cbuffer cbuff0 : register(b0)
{
	matrix world[512];
}

cbuffer cbuff1 : register(b1)
{
	matrix lightcamera;
}

//�{�[���̍ő吔
static const int MAX_BONES = 32;

cbuffer skinning : register(b2) //�{�[���̃X�L�j���O�s�񂪓���
{
	matrix matSkinning[MAX_BONES];
}

//�o�[�e�b�N�X�o�b�t�@�[�̓���
struct VSInput
{
	float4 pos : POSITION; //�ʒu
	float2 uv : TEXCOORD; //uv�l
	uint4 boneIndices : BONEINDICES; //�{�[���̔ԍ�
	float4 boneWeights : BONEWEIGHTS; //�{�[���̃X�L���E�F�C�g
	uint instanceID : SV_InstanceID;
};
struct InstanceData
{
	//matrix mat; // �R�c�ϊ��s��
	float4 color;
	matrix world; //���[���h�s��
};

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; //�r���[�v���W�F�N�V�����s��
	float3 cameraPos; //�J�������W�i���[���h���W�j
}

cbuffer cbuff1 : register(b1)
{
	InstanceData data[512];
}

cbuffer cbuff2 : register(b2)
{
	float3 m_ambient : packoffset(c0); // �A���r�G���g�W��
	float3 m_diffuse : packoffset(c1); // �f�B�t���[�Y�W��
	float3 m_specular : packoffset(c2); // �X�y�L�����[�W��
	float m_alpha : packoffset(c2.w); // �A���t�@
}

cbuffer cbuff3 : register(b3)
{
	float3 lightv; //���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor; //���C�g�̐F(RGB)
}

//�{�[���̍ő吔
static const int MAX_BONES = 32;

cbuffer skinning : register(b4) //�{�[���̃X�L�j���O�s�񂪓���
{
	matrix matSkinning[MAX_BONES];
}

cbuffer cbuff4 : register(b5)
{
	//�A���x�h
	float3 baseColor;
	//�����x
	float metalness;
	//���ʔ��˓x
	float specular;
	//�e��
	float roughness;
}

//�o�[�e�b�N�X�o�b�t�@�[�̓���
struct VSInput
{
	float4 pos : POSITION;	//�ʒu
	float3 normal : NORMAL;	//���_�@��
	float2 uv : TEXCOORD;	//�e�N�X�`�����W
	uint4 boneIndices : BONEINDICES;	//�{�[���̔ԍ�
	float4 boneWeights : BONEWEIGHTS;	//�{�[���̃X�L���E�F�C�g
	uint instanceID : SV_InstanceID;
};

//���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����
struct VSOutput
{
	float4 svpos : SV_POSITION;	//�V�X�e���p���_���W
	float4 worldpos : POSITION;	//���[���h���W
	float3 normal : NORMAL;		//�@��
	float2 uv : TEXCOORD;		//uv�l
	float4 color : COLOR;
};


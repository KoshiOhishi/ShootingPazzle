#include "Particle3D.hlsli"

//�l�p�`�̒��_��
static const uint vnum = 4;

//�Z���^�[����̃I�t�Z�b�g
static const float4 offset_array[vnum] =
{
	float4(-0.5f,-0.5f,0,0),	//����
	float4(-0.5f,+0.5f,0,0),	//����
	float4(+0.5f,-0.5f,0,0),	//�E��
	float4(+0.5f,+0.5f,0,0),	//�E��

};

//���オ0,0 �E����1,1
static const float2 uv_array[vnum] =
{
	float2(0, 1),	//����
	float2(0, 0),	//����
	float2(1, 1),	//�E��
	float2(1, 0),	//�E��
};

//�_�̓��͂���A�O�p�`���o��
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION, 
	//�����X�g���[��
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	//4�_���܂킷
	for (uint i = 0; i < vnum; i++)
	{
		//float4 offset = mul(matBillboard, offset_array[i]);
		//���S����̃I�t�Z�b�g���X�P�[�����O
		float4 offset = offset_array[i] * input[0].scale;
		//���S����̃I�t�Z�b�g���r���{�[�h��]�i���f�����W�j
		offset = mul(matBillboard, offset);
		//�I�t�Z�b�g�����炷�i���[���h���W�j
		element.svpos = input[0].pos + offset;
		//�r���[�v���W�F�N�V�����ϊ�
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		output.Append(element);
	}



	//for (uint i = 0; i < 3; i++)
	//{
	//	GSOutput element;	//�o�͗p���_�f�[�^
	//	element.svpos = input[i].svpos;	//���_���W���R�s�[
	//	element.normal = input[i].normal;//�@�����R�s�[
	//	element.uv = input[i].uv;	//UV���R�s�[
	//	//���_��1�o�́i�o�̓��X�g�ɒǉ��j
	//	output.Append(element);
	//}


	////�����̎n�_
	//element.svpos = input[0].svpos;
	//element.normal = input[0].normal;
	//element.uv = input[0].uv;
	//output.Append(element);

	////�����̏I�_
	//element.svpos = input[1].svpos;
	//element.normal = input[1].normal;
	//element.uv = input[1].uv;
	//output.Append(element);

	/*
	GSOutput element;

	//�O�p�`�̒��_1�_������
	for (uint i = 0; i < 3; i++)
	{
		//�����̎n�_
		element.svpos = input[i].svpos;	//���_���W���R�s�[
		element.normal = input[i].normal;//�@�����R�s�[
		element.uv = input[i].uv;	//UV���R�s�[
		//���_��1�o�́i�o�̓��X�g�ɒǉ��j
		output.Append(element);

		//�����̏I�_
		if (i == 2) {
			//+1����ƈ���̂ŁA�ŏ��ɖ߂�
			element.svpos = input[0].svpos;	//���_���W���R�s�[
			element.normal = input[0].normal;//�@�����R�s�[
			element.uv = input[0].uv;	//UV���R�s�[
		}
		else {
			element.svpos = input[i + 1].svpos;	//���_���W���R�s�[
			element.normal = input[i + 1].normal;//�@�����R�s�[
			element.uv = input[i + 1].uv;	//UV���R�s�[
		}
		output.Append(element);
		//���݂̃X�g���b�v���I�����A���̃X�g���b�v���J�n
		output.RestartStrip();
	}
	*/

	/*
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos = input[i].svpos;
		element.normal = input[i].normal;
		//UV��2�{��
		element.uv = input[i].uv * 2.0f;
		output.Append(element);
	}
	*/

	/*
	//1�ڂ̎O�p�`
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos = input[i].svpos;
		element.normal = input[i].normal;
		element.uv = input[i].uv;
		output.Append(element);
	}
	//���݂̃X�g���b�v���I��
	output.RestartStrip();

	//2�ڂ̎O�p�`
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		//X������20���炷
		element.svpos = input[i].svpos + float4(20.0f, 0.0f, 0.0f, 0.0f);;
		element.normal = input[i].normal;
		element.uv = input[i].uv * 5.0f;
		output.Append(element);
	}
	*/
}
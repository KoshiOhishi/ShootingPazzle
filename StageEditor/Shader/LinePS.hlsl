#include "Line.hlsli"

SamplerState smp : register(s0);		//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//�V�F�[�f�B���O�ɂ��F�ŕ`��
	return input.color;
}
#include "Line.hlsli"

VSOutput main(VSInput input)
{
	//�@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
	float4 wpos = mul(world, float4(input.pos, 1));

	//�s�N�Z���V�F�[�_�ɓn���l
	VSOutput output;
	output.pos = wpos;
	output.color = input.color;
	
	return output;
}
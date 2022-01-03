#include "ShadowTest.hlsli"

Texture2D<float> tex : register(t2);	//2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);		//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float colortex = pow(tex.Sample(smp, input.uv),20);
	return float4(colortex, colortex, colortex, 1);
}
#include "Line.hlsli"

SamplerState smp : register(s0);		//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//シェーディングによる色で描画
	return input.color;
}
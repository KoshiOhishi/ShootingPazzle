#include "InstancingOBJ.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float> lightDepthTex : register(t1); // 1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー
SamplerComparisonState shadowSmp : register(s1); //1番スロットに設定されたサンプラー

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input)
{
	PSOutput output;

	//テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	//シェーディングによる色
	float4 shadecolor;
	//光沢度
	const float shininess = 4.0f;
	//頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	//ライトに向かうベクトルと法線の計算
	float3 dotlightnormal = dot(lightv, input.normal);
	//反射光ベクトル
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	//環境反射光
	float3 ambient = m_ambient;
	//拡散反射光
	float3 diffuse = dotlightnormal * m_diffuse;
	//鏡面反射光
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	//全て加算する
	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = m_alpha;

	float3 posFromLightVP = input.tpos.xyz / input.tpos.w;
	float2 shadowUV = (posFromLightVP + float2(1, -1)) * float2(0.5, -0.5);
	float depthFromLight = lightDepthTex.SampleCmp(shadowSmp, shadowUV, posFromLightVP.z - 0.025f);
	float shadowWeight = lerp(0.5f, 1.0f, depthFromLight);
	
	//float depthFromLight = lightDepthTex.Sample(smp, shadowUV);
	
	////最大値を1とする
	//posFromLightVP.z = min(posFromLightVP.z, 1.0f);

	//float shadowWeight = step(posFromLightVP.z - 0.01f, depthFromLight);
	//shadowWeight = clamp(shadowWeight, 0.5f, 1.0f);

	//影有効なら0を、無効なら1を足して0～1にクリップ
	shadowWeight += (float) (1.0f - isDrawShadowToMyself);
	shadowWeight = clamp(shadowWeight, 0.0f, 1.0f);
	
	//シェーディングによる色で描画
	output.target0 = float4(shadecolor.rgb * texcolor.rgb * input.color.rgb * shadowWeight, shadecolor.a * texcolor.a * input.color.a);
	
	return output;
}
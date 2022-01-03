#include "FBX.hlsli"

//��
static const float PI = 3.141592654f;

//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex : register(t0);

//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerState smp : register(s0);


//�O���[�o���ϐ�
//���˓_�̖@���x�N�g��
static float3 N;


//Schlick�ɂ��ߎ�
//f0 �� f90 �̒l���ij - cosine�j��5���lerp����
//f0 : ���������ɓ��˂������̔��˗�
//f90 : �������s�ɓ��˂������̔��˗�
//cosine : 2�x�N�g���̂Ȃ��p�̃R�T�C���i���ϒl�j
float SchlickFresnel(float f0, float f90, float cosine)
{
	float m = saturate(1 - cosine);
	float m2 = m * m;
	float m5 = m2 * m2 * m;
	return lerp(f0, f90, m5);
}

float3 SchlickFresnel3(float3 f0, float3 f90, float cosine)
{
	float m = saturate(1 - cosine);
	float m2 = m * m;
	float m5 = m2 * m2 * m;
	return lerp(f0, f90, m5);
}

//�f�B�Y�j�[�̃t���l���v�Z
float3 DisneyFresnel(float LdotH)
{
	//F���i�t���l���FFresnel�j
	//�P�x
	float luminance = 0.3f * baseColor.r + 0.6f * baseColor.g + 0.1f * baseColor.b;
	//�F����
	float3 tintColor = baseColor / luminance;
	//������̋��ʔ��ːF���v�Z
	float3 nonMetalColor = specular * 0.08f * tintColor;
	//metalness�ɂ��F��ԁ@�����̏ꍇ�̓x�[�X�J���[
	float3 specularColor = lerp(nonMetalColor, baseColor, metalness);
	//NdotH�̊�����SchlickFresnel���
	return SchlickFresnel3(specularColor, float3(1, 1, 1), LdotH);

}

//UE4��GGX���z
// alpha : roughness��2��
// NdotH : �@���ƃn�[�t�x�N�g���̓���
float DistributionGGX(float alpha, float NdotH)
{
	float alpha2 = alpha * alpha;
	float t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (PI * t * t);
}

//UE4��Smith���f��
float GeometricSmith(float cosine)
{
	float k = (roughness + 1.0f);
	
	k = k * k / 8.0f;
	
	return cosine / (cosine * (1.0f - k) + k);

}
//���ʔ��˂̌v�Z
float3 CookTorranceSpecular(float NdotL, float NdotV, float NdotH, float LdotH)
{
	//D���i���z�FDistribution�j
	float Ds = DistributionGGX(roughness * roughness, NdotH);

	//F���i�t���l���FFresnel�j
	float3 Fs = DisneyFresnel(LdotH);

	//G���i�􉽌����FGeometry attenuaion�j
	float Gs = GeometricSmith(NdotL) * GeometricSmith(NdotV);

	//m���i����j
	float m = 4.0f * NdotL * NdotV;

	//�������ċ��ʔ��˂̐F�𓾂�
	return Ds * Fs * Gs / m;
}

//�o�������˕��z�֐�
float3 BRDF(float3 L, float3 V)
{
	//�@���ƃ��C�g�����̓���
	float NdotL = dot(N, L);
	//�@���ƃJ���������̓���
	float NdotV = dot(N, V);
	//�ǂ��炩��90�x�ȏ�ł���ΐ^������Ԃ�
	if (NdotL < 0 || NdotV < 0) { return float3(0, 0, 0); }

	//���C�g�����ƃJ���������̒��ԁi�n�[�t�x�N�g���j
	float3 H = normalize(L + V);
	//�@���ƃn�[�t�x�N�g���̓���
	float NdotH = dot(N, H);
	//���C�g�ƃn�[�t�x�N�g���̓���
	float LdotH = dot(L, H);

	//�g�U���˗�
	float diffuseReflectance = 1.0f / PI;

	float energyBias = 0.5f * roughness;

	//���ˊp��90�x�̏ꍇ�̊g�U���˗�
	float Fd90 = energyBias + 2 * LdotH * LdotH * roughness;
	//�����Ă������̊g�U���˗�
	float FL = SchlickFresnel(1.0f, Fd90, NdotL);
	//�o�Ă������̊g�U���˗�
	float FV = SchlickFresnel(1.0f, Fd90, NdotV);

	float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);

	//�����ďo�Ă����܂ł̊g�U���˗�
	float Fd = FL * FV * energyFactor;

	//�g�U���ˍ�
	float3 diffuseColor = diffuseReflectance * Fd * baseColor * (1 - metalness);

	//���ʔ��ˍ�
	float3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH);

	//�g�U���˂Ƌ��ʔ��˂̍��v�ŐF�����܂�
	return diffuseColor + specularColor;
}

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

//�G���g���[�|�C���g
PSOutput main(VSOutput input)
{
	PSOutput output;

	//�e�N�X�`���}�b�s���O
	float4 texcolor = tex.Sample(smp, input.uv);

	//�V�F�[�f�B���O�ɂ��F
	float4 shadecolor;
	//����x
	const float shininess = 4.0f;
	//���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	//���C�g�Ɍ������x�N�g���Ɩ@���̌v�Z
	float3 dotlightnormal = dot(lightv, input.normal);
	//���ˌ��x�N�g��
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	//�����ˌ�
	float3 ambient = m_ambient;
	//�g�U���ˌ�
	float3 diffuse = dotlightnormal * m_diffuse;
	//���ʔ��ˌ�
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	//�S�ĉ��Z����
	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = m_alpha;


	//�ʂ̏���static�ϐ��ɑ�����A�֐�����Q�Ƃł���悤�ɂ���
	N = input.normal;
	//�ŏI�I��RGB
	float3 finalRGB = float3(0, 0, 0);
	//���_���王���ւ̕����x�N�g��
	//float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	//���s����
	finalRGB += BRDF(lightv, eyedir) * lightcolor;


	output.target0 = float4(finalRGB, 1);
	output.target1 = float4(finalRGB, 1);

	return output;
}
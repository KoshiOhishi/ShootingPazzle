struct InstanceData
{
	//matrix mat; // ３Ｄ変換行列
	float4 color;
	matrix world; //ワールド行列
};

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; //ビュープロジェクション行列
	matrix lightcamera;
	uint isDrawShadowToMyself; //他のオブジェクトの影を自分に描画するか（0...しない 1...する）
	float3 cameraPos; //カメラ座標（ワールド座標）
}

cbuffer cbuff1 : register(b1)
{
	InstanceData data;
}

cbuffer cbuff2 : register(b2)
{
	float3 m_ambient : packoffset(c0); // アンビエント係数
	float3 m_diffuse : packoffset(c1); // ディフューズ係数
	float3 m_specular : packoffset(c2); // スペキュラー係数
	float m_alpha : packoffset(c2.w); // アルファ
}

cbuffer cbuff3 : register(b3)
{
	float3 lightv; //ライトへの方向の単位ベクトル
	float3 lightcolor; //ライトの色(RGB)
}

//ボーンの最大数
static const int MAX_BONES = 32;

cbuffer skinning : register(b4) //ボーンのスキニング行列が入る
{
	matrix matSkinning[MAX_BONES];
}

cbuffer cbuff4 : register(b5)
{
	//アルベド
	float3 baseColor;
	//金属度
	float metalness;
	//鏡面反射度
	float specular;
	//粗さ
	float roughness;
}

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos : POSITION;	//位置
	float3 normal : NORMAL;	//頂点法線
	float2 uv : TEXCOORD;	//テクスチャ座標
	uint4 boneIndices : BONEINDICES;	//ボーンの番号
	float4 boneWeights : BONEWEIGHTS;	//ボーンのスキンウェイト
};

//頂点シェーダーからピクセルシェーダーへのやり取りに使用する
struct VSOutput
{
	float4 svpos : SV_POSITION;	//システム用頂点座標
	float4 worldpos : POSITION;	//ワールド座標
	float4 tpos : TPOS;			//ライトビュー座標
	float3 normal : NORMAL;		//法線
	float2 uv : TEXCOORD;		//uv値
};


#include "Particle3D.hlsli"

//四角形の頂点数
static const uint vnum = 4;

//センターからのオフセット
static const float4 offset_array[vnum] =
{
	float4(-0.5f,-0.5f,0,0),	//左下
	float4(-0.5f,+0.5f,0,0),	//左上
	float4(+0.5f,-0.5f,0,0),	//右下
	float4(+0.5f,+0.5f,0,0),	//右上

};

//左上が0,0 右下が1,1
static const float2 uv_array[vnum] =
{
	float2(0, 1),	//左下
	float2(0, 0),	//左上
	float2(1, 1),	//右下
	float2(1, 0),	//右上
};

//点の入力から、三角形を出力
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION, 
	//線分ストリーム
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	//4点分まわす
	for (uint i = 0; i < vnum; i++)
	{
		//float4 offset = mul(matBillboard, offset_array[i]);
		//中心からのオフセットをスケーリング
		float4 offset = offset_array[i] * input[0].scale;
		//中心からのオフセットをビルボード回転（モデル座標）
		offset = mul(matBillboard, offset);
		//オフセット分ずらす（ワールド座標）
		element.svpos = input[0].pos + offset;
		//ビュープロジェクション変換
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		output.Append(element);
	}



	//for (uint i = 0; i < 3; i++)
	//{
	//	GSOutput element;	//出力用頂点データ
	//	element.svpos = input[i].svpos;	//頂点座標をコピー
	//	element.normal = input[i].normal;//法線をコピー
	//	element.uv = input[i].uv;	//UVをコピー
	//	//頂点を1つ出力（出力リストに追加）
	//	output.Append(element);
	//}


	////線分の始点
	//element.svpos = input[0].svpos;
	//element.normal = input[0].normal;
	//element.uv = input[0].uv;
	//output.Append(element);

	////線分の終点
	//element.svpos = input[1].svpos;
	//element.normal = input[1].normal;
	//element.uv = input[1].uv;
	//output.Append(element);

	/*
	GSOutput element;

	//三角形の頂点1点ずつ扱う
	for (uint i = 0; i < 3; i++)
	{
		//線分の始点
		element.svpos = input[i].svpos;	//頂点座標をコピー
		element.normal = input[i].normal;//法線をコピー
		element.uv = input[i].uv;	//UVをコピー
		//頂点を1つ出力（出力リストに追加）
		output.Append(element);

		//線分の終点
		if (i == 2) {
			//+1すると溢れるので、最初に戻る
			element.svpos = input[0].svpos;	//頂点座標をコピー
			element.normal = input[0].normal;//法線をコピー
			element.uv = input[0].uv;	//UVをコピー
		}
		else {
			element.svpos = input[i + 1].svpos;	//頂点座標をコピー
			element.normal = input[i + 1].normal;//法線をコピー
			element.uv = input[i + 1].uv;	//UVをコピー
		}
		output.Append(element);
		//現在のストリップを終了し、次のストリップを開始
		output.RestartStrip();
	}
	*/

	/*
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos = input[i].svpos;
		element.normal = input[i].normal;
		//UVを2倍に
		element.uv = input[i].uv * 2.0f;
		output.Append(element);
	}
	*/

	/*
	//1つ目の三角形
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos = input[i].svpos;
		element.normal = input[i].normal;
		element.uv = input[i].uv;
		output.Append(element);
	}
	//現在のストリップを終了
	output.RestartStrip();

	//2つ目の三角形
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		//X方向に20ずらす
		element.svpos = input[i].svpos + float4(20.0f, 0.0f, 0.0f, 0.0f);;
		element.normal = input[i].normal;
		element.uv = input[i].uv * 5.0f;
		output.Append(element);
	}
	*/
}
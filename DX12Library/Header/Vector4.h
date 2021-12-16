#pragma once
#include <DirectXMath.h>

struct Vector4 : public DirectX::XMFLOAT4
{
	//コンストラクタ
	Vector4();								//零ベクトルとする
	Vector4(float x, float y, float z, float w);		//x成分、y成分、z成分を指定しての生成

	/// <summary>
	/// 零ベクトルを返す
	/// </summary>
	/// <returns></returns>
	const Vector4& Zero() const;

	//単項演算子オーバーロード
	Vector4 operator+() const;
	Vector4 operator-() const;

	//代入演算子オーバーロード
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
};

//2項演算子オーバーロード
//いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);


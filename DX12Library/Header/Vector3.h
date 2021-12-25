#pragma once
#include <DirectXMath.h>

struct Vector3 : public DirectX::XMFLOAT3
{

	//コンストラクタ
	Vector3();								//零ベクトルとする
	Vector3(float x, float y, float z);		//x成分、y成分、z成分を指定しての生成

	//メンバ関数

	/// <summary>
	/// ノルムを求める
	/// </summary>
	/// <returns>ノルム(長さ)</returns>
	const float Length() const;

	/// <summary>
	/// ノルムの2乗を返す (当たり判定用)
	/// </summary>
	/// <returns></returns>
	const float LengthSq() const;

	/// <summary>
	/// 正規化したベクトルを返す
	/// </summary>
	/// <returns>正規化したベクトル</returns>
	const Vector3 Normalize() const;

	/// <summary>
	/// 内積を求める
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>内積</returns>
	const float Dot(const Vector3& v) const;

	/// <summary>
	/// 外積を求める
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>外積</returns>
	const Vector3 Cross(const Vector3& v) const;

	/// <summary>
	/// 零ベクトルを取得
	/// </summary>
	/// <returns></returns>
	static const Vector3 Zero();

	//単項演算子オーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;

	//代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2項演算子オーバーロード
//いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);



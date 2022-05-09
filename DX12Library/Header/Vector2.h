#pragma once
#include <DirectXMath.h>

namespace DX12Library {

	struct Vector2 : public DirectX::XMFLOAT2
	{
		//コンストラクタ
		Vector2();								//零ベクトルとする
		Vector2(float x, float y);		//x成分、y成分を指定しての生成

			//メンバ関数

		/// <summary>
		/// ノルムを求める
		/// </summary>
		/// <returns>ノルム(長さ)</returns>
		const float Length() const;

		/// <summary>
		/// 正規化したベクトルを返す
		/// </summary>
		/// <returns>正規化したベクトル</returns>
		const Vector2 Normalize() const;

		/// <summary>
		/// 内積を求める
		/// </summary>
		/// <param name="v">ベクトル</param>
		/// <returns>内積</returns>
		const float Dot(const Vector2& v) const;

		/// <summary>
		/// 外積を求める
		/// </summary>
		/// <param name="v">ベクトル</param>
		/// <returns>外積</returns>
		const float Cross(const Vector2& v) const;

		/// <summary>
		/// 零ベクトルを取得
		/// </summary>
		/// <returns></returns>
		static const Vector2 Zero();

		//単項演算子オーバーロード
		Vector2 operator+() const;
		Vector2 operator-() const;

		//代入演算子オーバーロード
		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator*=(float s);
		Vector2& operator/=(float s);
	};
}

using Vector2 = DX12Library::Vector2;
//2項演算子オーバーロード
//いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);
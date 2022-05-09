#pragma once
#include "Vector3.h"
#include "Matrix4.h"

namespace DX12Library {

	struct Quaternion
	{
		float x;	//x成分
		float y;	//y成分
		float z;	//z成分
		float w;	//w成分

		//コンストラクタ
		Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
		Quaternion(const Vector3& v, float angle);
		Quaternion(const Vector3& v, const Quaternion& q);
		Quaternion(const Matrix4& m);

		/// <summary>
		/// 値を指定して作成したクォータニオンを返す
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		/// <returns>クォータニオン</returns>
		static const Quaternion CreateQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

		/// <summary>
		/// 任意軸まわりの回転の指定から作成したクォータニオンを返す
		/// </summary>
		/// <param name="v">回転するベクトル</param>
		/// <param name="angle">角度(Digrees)</param>
		/// <returns>クォータニオン</returns>
		static const Quaternion CreateQuaternion(const Vector3& v, float angle);

		/// <summary>
		/// ベクトルを回転したクォータニオンを返す
		/// </summary>
		/// <param name="v">回転するベクトル</param>
		/// <param name="q">クォータニオン</param>
		/// <returns>クォータニオン</returns>
		static const Quaternion CreateQuaternion(const Vector3& v, const Quaternion& q);

		/// <summary>
		/// 回転行列から作成したクォータニオンを返す
		/// </summary>
		/// <param name="m">回転行列</param>
		/// <returns>クォータニオン</returns>
		static const Quaternion CreateQuaternion(const Matrix4& m);

		/// <summary>
		/// 内積を求める
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>内積</returns>
		const float Dot(const Quaternion& q) const;

		/// <summary>
		/// ノルムを求める
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>ノルム(長さ)</returns>
		const float Length() const;

		/// <summary>
		/// 正規化する
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>正規化したクォータニオン</returns>
		const Quaternion Normalize()const;

		/// <summary>
		/// 共役四元数を求める
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>共役四元数</returns>
		const Quaternion Conjugate()const;

		/// <summary>
		/// 球面補間したクォータニオンを返す
		/// </summary>
		/// <param name="q1">開始クォータニオン</param>
		/// <param name="q2">終了クォータニオン</param>
		/// <param name="t">パラメータ (0～1)</param>
		/// <returns>クォータニオン</returns>
		static const Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float t);

		/// <summary>
		/// 線形補間したクォータニオンを返す
		/// </summary>
		/// <param name="q1">開始クォータニオン</param>
		/// <param name="q2">終了クォータニオン</param>
		/// <param name="t">パラメータ (0～1)</param>
		/// <returns>クォータニオン</returns>
		static const Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

		/// <summary>
		/// クォータニオンから回転した行列を返す
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>クォータニオン</returns>
		static const Matrix4 Rotate(const Quaternion& q);

		/// <summary>
		/// 回転軸の算出
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>回転軸ベクトル</returns>
		static const Vector3 GetAxis(const Quaternion& q);
	};
}

using Quaternion = DX12Library::Quaternion;
//単項演算子オーバーロード
Quaternion operator+(const Quaternion& q);
Quaternion operator-(const Quaternion& q);

//代入演算子オーバーロード
Quaternion& operator+=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator-=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator*=(Quaternion& q, float s);
Quaternion& operator/=(Quaternion& q, float s);
Quaternion& operator*=(Quaternion& q1, const Quaternion& q2);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q, float s);
Quaternion operator*(float s, const Quaternion& q);
Quaternion operator/(const Quaternion& q, float s);

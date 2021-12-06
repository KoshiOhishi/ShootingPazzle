#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

// MATRIX 型を Matrix4 型として扱う ※互換性の確保&拡張のため
using Matrix4 = DirectX::XMMATRIX;

//単位行列を求める
Matrix4 Identity();

//拡大縮小行列の設定
Matrix4 Scale(const Vector3& s);

//回転行列の設定
Matrix4 RotateX(float angle);
Matrix4 RotateY(float angle);
Matrix4 RotateZ(float angle);

//平行移動行列の作成
Matrix4 Translate(const Vector3& t);

//座標変換（ベクトルと行列の掛け算をする）
Vector3 Transform(const Vector3& v, const Matrix4& m);


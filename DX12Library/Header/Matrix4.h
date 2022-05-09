#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace DX12Library {

	using Matrix4 = DirectX::XMMATRIX;

	//単位行列を求める
	Matrix4 Matrix4Identity();

	//拡大縮小行列の設定
	Matrix4 Matrix4Scale(const Vector3& s);

	//回転行列の設定
	Matrix4 Matrix4RotateX(float angle);
	Matrix4 Matrix4RotateY(float angle);
	Matrix4 Matrix4RotateZ(float angle);

	//平行移動行列の作成
	Matrix4 Matrix4Translate(const Vector3& t);

	//座標変換（ベクトルと行列の掛け算をする）
	Vector3 Matrix4Transform(const Vector3& v, const Matrix4& m);

}
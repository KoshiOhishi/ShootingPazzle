#pragma once

#include "BaseCollider.h"
#include <DirectXMath.h>
#include "Vector3.h"

class Object3D;

/// <summary>
/// レイキャストによる情報を得る為の構造体
/// </summary>
struct RaycastHit
{
	// 衝突相手のオブジェクト
	Object3D* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	Vector3 inter;
	// 衝突点までの距離
	float distance = 0.0f;
};


#pragma once
/// <summary>
/// 当たり判定プリミティブ
/// </summary>

#include <DirectXMath.h>
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"

/// <summary>
/// 球
/// </summary>
struct Sphere
{
	// 中心座標
	Vector3 center = {};
	// 半径
	float radius = 1.0f;
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
	// 法線ベクトル
	Vector3 normal = { 0,1,0 };
	// 原点(0,0,0)からの距離
	float distance = 0.0f;
};

/// <summary>
/// レイ（半直線）
/// </summary>
struct Ray
{
	//始点座標
	Vector3 start = { 0,0,0 };
	//方向
	Vector3 dir = { 1,0,0 };
};

/// <summary>
/// 軸に平行な直方体
/// </summary>
struct AABB
{
	//左下手前座標
	Vector3 min = { 0, 0, 0 };
	//右上奥座標
	Vector3 max = { 0, 0, 0 };
};

/// <summary>
/// 法線付き三角形（時計回りが表面）
/// </summary>
class Triangle
{
public:
	//頂点座標3つ
	Vector3 p0;
	Vector3 p1;
	Vector3 p2;
	//法線ベクトル
	Vector3 normal;

	/// <summary>
	/// 法線の計算
	/// </summary>
	void ComputeNormal();
};

struct Capsule
{
	Vector3 start = { -1,0,0 };
	Vector3 end = { 1,0,0 };
	float radius = 0.5;
};

struct Point
{
	Vector3 pos;
};
#pragma once

#include "CollisionPrimitive.h"

namespace DX12Library {

	/// <summary>
	/// 当たり判定ヘルパークラス
	/// </summary>
	class Collision
	{
	public:

		/// <summary>
		/// 球と球の当たり判定
		/// </summary>
		/// <param name="sphere1">球1</param>
		/// <param name="sphere2">球2</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckSphere2Sphere(const Sphere& sphere1, const Sphere& sphere2, Vector3* inter = nullptr);

		/// <summary>
		/// 球と平面の当たり判定
		/// </summary>
		/// <param name="sphere">球</param>
		/// <param name="plane">平面</param>
		/// <param name="inter">交点（平面上の再接近点）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

		/// <summary>
		/// 点と三角形の最近接点を求める
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="triangle">三角形</param>
		/// <param name="closest">最近接点</param>
		static void ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

		/// <summary>
		/// 球と法線付き三角形の当たり判定チェック
		/// </summary>
		/// <param name="sphere">球</param>
		/// <param name="triangle">三角形</param>
		/// <param name="inter">交点（三角形上の最近接点）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr);

		/// <summary>
		/// レイと平面の当たり判定
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="plane">平面</param>
		/// <param name="distance">距離（出力用）</param>
		/// <param name="inter">交点（距離用）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

		/// <summary>
		/// レイと法線付き三角形の当たり判定
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="triangle">三角形</param>
		/// <param name="distance">距離（出力用）</param>
		/// <param name="inter">交点（距離用）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);

		/// <summary>
		/// レイと球の当たり判定
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="sphere">球</param>
		/// <param name="distance">距離（出力用）</param>
		/// <param name="inter">交点（距離用）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);

		/// <summary>
		/// レイとAABBの当たり判定
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="sphere">AABB</param>
		/// <param name="distance">距離（出力用）</param>
		/// <param name="inter">交点（距離用）</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckRay2AABB(const Ray& ray, const AABB& aabb, float* distance = nullptr, Vector3* inter = nullptr);

		/// <summary>
		/// AABB同士の当たり判定
		/// </summary>
		/// <param name="box1">直方体1</param>
		/// <param name="box2">直方体2</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckAABB2AABB(const AABB& aabb1, const AABB& aabb2, Vector3* inter = nullptr);

		/// <summary>
		/// AABBと球の当たり判定
		/// </summary>
		/// <param name="box"></param>
		/// <param name="sphere"></param>
		/// <returns>交差しているか否か</returns>
		static bool CheckAABB2Sphere(const AABB& aabb, const Sphere& sphere);

		/// <summary>
		/// レイと球の衝突座標を算出
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="sphere">球</param>
		/// <param name="start">衝突開始点</param>
		/// <param name="end">衝突終了点</param>
		/// <returns>衝突判定</returns>
		static bool CalcInterRay2Sphere(const Ray& ray, const Sphere& sphere, Vector3* start = nullptr, Vector3* end = nullptr);

		/// <summary>
		/// レイと無限円柱の衝突座標を算出
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="cilStart">円柱軸の1点</param>
		/// <param name="cilEnd">円柱軸のもう1点</param>
		/// <param name="cilRadius">円柱の半径</param>
		/// <param name="start">衝突開始点</param>
		/// <param name="end">衝突終了点</param>
		/// <returns>衝突判定</returns>
		static bool CalcInterRay2InfCilinder(const Ray& ray, const Vector3& cilStart, const Vector3& cilEnd, float cilRadius, Vector3* start = nullptr, Vector3* end = nullptr);

		/// <summary>
		/// レイとカプセルの当たり判定
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="capsule">カプセル</param>
		/// <param name="distance">距離（出力用）</param>
		/// <param name="start">衝突開始点</param>
		/// <param name="end">衝突終了点</param>
		/// <param name="normal">レイが当たった面の法線</param>
		/// <returns>交差しているか否か</returns>
		static bool CheckRay2Capsule(const Ray& ray, const Capsule& capsule, float* distance = nullptr, Vector3* start = nullptr, Vector3* end = nullptr, Vector3* normal = nullptr);

	private:
		/// <summary>
		/// ∠p1p2p3の内積を算出
		/// </summary>
		/// <param name="p1"></param>
		/// <param name="p2"></param>
		/// <param name="p3"></param>
		/// <returns>内積</returns>
		static float CheckDot(const Vector3& p1, const Vector3& p2, const Vector3& p3);
	};

}
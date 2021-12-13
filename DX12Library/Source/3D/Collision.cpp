#include "Collision.h"
#include "Object3D.h"
#include "ObjModel.h"

using namespace DirectX;

bool Collision::CheckSphere2Sphere(const Sphere& sphere1, const Sphere& sphere2, Vector3* inter)
{
	//半径の合計
	float len2 = sphere1.radius + sphere2.radius;
	len2 *= len2;
	//
	Vector3 position_sub = (sphere1.center - sphere2.center);

	float distance = position_sub.LengthSq();

	if (distance > len2)
		return false;

	if (inter) {
		// Aの半径が0の時座標はBの中心　Bの半径が0の時座標はAの中心　となるよう補完
		float t = sphere2.radius / (sphere1.radius + sphere2.radius);
		*inter = sphere1.center + t * (sphere2.center - sphere1.center);
	}

	return true;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter)
{
	// 座標系の原点から球の中心座標への距離
	float distV = sphere.center.Dot(plane.normal); //XMVector3Dot(sphere.center, plane.normal);	
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV - plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius)	return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の再接近点を、疑似交点とする
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	Vector3 p0_p2 = triangle.p2 - triangle.p0;
	Vector3 p0_pt = point - triangle.p0;

	float d1 = p0_p1.Dot(p0_pt);
	float d2 = p0_p2.Dot(p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	Vector3 p1_pt = point - triangle.p1;

	float d3 = p0_p1.Dot(p1_pt);
	float d4 = p0_p2.Dot(p1_pt);

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	Vector3 p2_pt = point - triangle.p2;

	float d5 = p0_p1.Dot(p2_pt);
	float d6 = p0_p2.Dot(p2_pt);
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter)
{
	Vector3 p;
	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	//点pと球の中心の差分ベクトル
	Vector3 v = p - sphere.center;
	//距離の二乗を求める
	//（同じベクトル同士の内積は三平方の定理のルート内部の式と一致する）
	float d = v.Dot(v);
	//球と三角形の距離が半径以下なら当たっていない
	if (d > sphere.radius * sphere.radius) return false;
	//疑似交点を計算
	if (inter) {
		//三角形上の最近接点pを疑似交点とする
		*inter = p;
	}

	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float * distance, Vector3* inter)
{
	const float epsilon = 1.0e-5f;	//誤差吸収用の微小な値
	// 面法線とレイの方向ベクトルの内積
	float d1 = plane.normal.Dot(ray.dir);
	// 裏面には当たらない
	if (d1 > -epsilon) { return false; }
	// 始点と原点の距離（平面の法線方向）
	// 面法線とレイの始点座標（位置ベクトル）の内積
	float d2 = plane.normal.Dot(ray.start);
	// 始点と平面の距離（平面の法線方向）
	float dist = d2 - plane.distance;
	// 始点と平面の距離（レイ方向）
	float t = dist / -d1;
	//交点が始点より後ろにあるので、当たらない
	if (t < 0) return false;
	//距離を書き込む
	if (distance) { *distance = t; }
	//交点を計算
	if (inter) { *inter = ray.start + t * ray.dir; }
	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float * distance, Vector3* inter)
{
	//三角形が乗っている平面を算出
	Plane plane;
	Vector3 interPlane;
	plane.normal = triangle.normal;
	plane.distance = triangle.normal.Dot(triangle.p0);
	//レイと平面が当たっていなければ、当たっていない
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) { return false; }
	//レイと平面が当たっていたので、距離と交点が書き込まれた
	//レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;	//誤差吸収用の微小な値

	Vector3 m = {};

	//辺p0_p1について
	Vector3 pt_p0 = triangle.p0 - interPlane;
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	m = pt_p0.Cross(p0_p1);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(triangle.normal) < -epsilon) { return false; }

	//辺p1_p2について
	Vector3 pt_p1 = triangle.p1 - interPlane;
	Vector3 p1_p2 = triangle.p2 - triangle.p1;
	m = pt_p1.Cross(p1_p2);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(triangle.normal) < -epsilon) { return false; }

	//辺p2_p0について
	Vector3 pt_p2 = triangle.p2 - interPlane;
	Vector3 p2_p0 = triangle.p0 - triangle.p2;
	m = pt_p2.Cross(p2_p0);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(triangle.normal) < -epsilon) { return false; }

	//内側なので、当たっている
	if (inter) {
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float * distance, Vector3* inter)
{
	Vector3 m = ray.start - sphere.center;
	float b = m.Dot(ray.dir);
	float c = m.Dot(m) - sphere.radius * sphere.radius;
	//layの始点がsphereの外側にあり(c > 0)、layがsphereから離れていく方向を指している場合は(b > 0)、当たらない
	if (c > 0.0f && b > 0.0f) return false;

	float discr = b * b - c;
	//負の判別式はレイが球を外れていることに一致
	if (discr < 0.0f) { return false; }

	//レイは球と交差している。
	//交差する最小の値tを計算
	float t = -b - sqrt(discr);
	//tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if (t < 0) t = 0.0f;
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2AABB(const Ray& ray, const AABB& aabb, float* distance, Vector3* inter)
{
	//http://marupeke296.com/COL_3D_No18_LineAndAABB.html より

	float t = -FLT_MAX;
	float t_max = FLT_MAX;

	float rayPos[3], rayDir[3], min[3], max[3];
	memcpy(rayPos, &ray.start, sizeof(Vector3));
	memcpy(rayDir, &ray.dir, sizeof(Vector3));
	memcpy(min, &aabb.min, sizeof(Vector3));
	memcpy(max, &aabb.max, sizeof(Vector3));

	for (int i = 0; i < 3; i++) {
		if (abs(rayDir[i]) < FLT_EPSILON) {
			if (rayPos[i] < min[i] || rayPos[i] > max[i]) {
				return false;
			}
		}
		else {
			//スラブとの距離を算出
			//t1が近スラブ、t2が遠スラブとの距離
			float odd = 1.0f / rayDir[i];
			float t1 = (min[i] - rayPos[i]) * odd;
			float t2 = (max[i] - rayPos[i]) * odd;
			if (t1 > t2) {
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}

			if (t1 > t) t = t1;
			if (t2 < t_max) t_max = t2;

			//スラブ交差チェック
			if (t >= t_max) {
				return false;
			}
		}
	}

	//交差している
	if (distance) { *distance = t; }

	if (inter) {
		*inter = ray.start + t * ray.dir;
	}

	return true;
}

bool Collision::CheckAABB2AABB(const AABB& aabb1, const AABB& aabb2, Vector3* inter)
{
	//条件にすべて当てはまったら衝突
	bool col[6];

	//obj1の右側 > obj2の左側
	col[0] = aabb1.max.x > aabb2.min.x;

	//obj1の左側 < obj2の右側
	col[1] = aabb1.min.x < aabb2.max.x;

	//obj1の上側 > obj2の下側
	col[2] = aabb1.max.y > aabb2.min.y;

	//obj1の下側 < obj2の上側
	col[3] = aabb1.min.y < aabb2.max.y;

	//obj1の奥側 > obj2の手前側
	col[4] = aabb1.max.z > aabb2.min.z;

	//obj1の手前側 < obj2の奥側
	col[5] = aabb1.min.z < aabb2.max.z;

	if (col[0] && col[1] && col[2] && col[3] && col[4] && col[5]) {

		//交点算出
		if (inter) {
			Vector3 tmp = {};
			if (aabb1.min.x < aabb2.min.x) {
				tmp.x = aabb1.max.x - (aabb1.max.x - aabb2.min.x) * 0.5f;
			}
			else {
				tmp.x = aabb2.max.x - (aabb2.max.x - aabb1.min.x) * 0.5f;
			}
			if (aabb1.min.y < aabb2.min.y) {
				tmp.y = aabb1.max.y - (aabb1.max.y - aabb2.min.y) * 0.5f;
			}
			else {
				tmp.y = aabb2.max.y - (aabb2.max.y - aabb1.min.y) * 0.5f;
			}
			if (aabb1.min.z < aabb2.min.z) {
				tmp.z = aabb1.max.z - (aabb1.max.z - aabb2.min.z) * 0.5f;
			}
			else {
				tmp.z = aabb2.max.z - (aabb2.max.z - aabb1.min.z) * 0.5f;
			}

			*inter = tmp;
		}
		return true;
	}

	return false;
}

bool Collision::CheckAABB2Sphere(const AABB& aabb, const Sphere& sphere)
{
	//球の中心座標とAABBとの距離をdとし、
	//球の半径をrとすると
	//d < r の時　衝突している
	//(d² < r²)

	//球の中心座標とAABBとの距離
	float d2 = 0;

	//球の中心座標とAABBとの距離を求める

	//x軸方向の距離の2乗を加算
	if (sphere.center.x < aabb.min.x) {
		d2 += (sphere.center.x - aabb.min.x) * (sphere.center.x - aabb.min.x);
	}
	if (sphere.center.x > aabb.max.x) {
		d2 += (sphere.center.x - aabb.max.x) * (sphere.center.x - aabb.max.x);
	}
	//y軸方向の距離の2乗を加算
	if (sphere.center.y < aabb.min.y) {
		d2 += (sphere.center.y - aabb.min.y) * (sphere.center.y - aabb.min.y);
	}
	if (sphere.center.y > aabb.max.y) {
		d2 += (sphere.center.y - aabb.max.y) * (sphere.center.y - aabb.max.y);
	}
	//z軸方向の距離の2乗を加算
	if (sphere.center.z < aabb.min.z) {
		d2 += (sphere.center.z - aabb.min.z) * (sphere.center.z - aabb.min.z);
	}
	if (sphere.center.z > aabb.max.z) {
		d2 += (sphere.center.z - aabb.max.z) * (sphere.center.z - aabb.max.z);
	}

	//(d2 < r * r)のとき、衝突
	if (d2 < sphere.radius * sphere.radius) {
		return true;
	}

	return false;

}

bool Collision::CalcInterRay2Sphere(const Ray& ray, const Sphere& sphere, Vector3* start, Vector3* end)
{
	Vector3 p = sphere.center - ray.start;

	float a = ray.dir.LengthSq();
	float b = ray.dir.Dot(p);
	float c = p.LengthSq() - sphere.radius * sphere.radius;

	if (a == 0.0f) {
		return false;	//レイの長さが0
	}

	float s = b * b - a * c;
	if (s < 0.0f) {
		return false;	//衝突していない
	}

	s = sqrtf(s);
	float a1 = (b - s) / a;
	float a2 = (b + s) / a;

	if (a1 < 0.0f || a2 < 0.0f) {
		return false;	//レイの反対で衝突
	}

	if (start) {
		*start = ray.start + a1 * ray.dir;
	}
	if (end) {
		*end = ray.start + a2 * ray.dir;
	}

	return true;
}

bool Collision::CalcInterRay2InfCilinder(const Ray& ray, const Vector3& cilStart, const Vector3& cilEnd, float cilRadius, Vector3* start, Vector3* end)
{
	Vector3 p1 = cilStart - ray.start;
	Vector3 p2 = cilEnd - ray.start;
	Vector3 s = p2 - p1;

	//各種内積値
	float dvv = ray.dir.LengthSq();
	float dsv = s.Dot(ray.dir);
	float dpv = p1.Dot(ray.dir);
	float dss = s.LengthSq();
	float dps = p1.Dot(s);
	float dpp = p1.LengthSq();
	float rr = cilRadius * cilRadius;

	if (dss == 0.0f) {
		return false; //円柱が定義されない
	}

	float a = dvv - dsv * dsv / dss;
	float b = dpv - dps * dsv / dss;
	float c = dpp - dps * dps / dss - rr;

	if (a == 0.0f) {
		return false;
	}

	float fs = b * b - a * c;
	if (fs < 0.0f) {
		return false;	//レイが円柱と衝突していない
	}
	fs = sqrtf(fs);

	float a1 = (b - fs) / a;
	float a2 = (b + fs) / a;

	if (a1 < 0.0f || a2 < 0.0f) {
		return false;	//レイの反対で衝突
	}

	if (start) {
		*start = ray.start + a1 * ray.dir;
	}
	if (end) {
		*end = ray.start + a2 * ray.dir;
	}

	return true;
}

bool Collision::CheckRay2Capsule(const Ray& ray, const Capsule& capsule, float* distance, Vector3* start, Vector3* end, Vector3* normal)
{
	bool Q1inP1 = false;
	bool Q1inP2 = false;
	bool Q1inCld = false;
	bool result = false;

	Vector3 tmp_start;
	Vector3 tmp_end;

	//Q1の検査
	Sphere p1 = { capsule.start, capsule.radius };
	Sphere p2 = { capsule.end, capsule.radius };
	if (CalcInterRay2Sphere(ray, p1, &tmp_start, &tmp_end) == true &&
		CheckDot(capsule.end, capsule.start, tmp_start) <= 0.0f) {
		Q1inP1 = true;	//Q1は球面P1上にある
	}
	else if (CalcInterRay2Sphere(ray, p2, &tmp_start, &tmp_end) == true &&
		CheckDot(capsule.start, capsule.end, tmp_start) <= 0.0f) {
		Q1inP2 = true;	//Q1は球面P2上にある
	}
	else if (CalcInterRay2InfCilinder(ray, capsule.start, capsule.end, capsule.radius, &tmp_start, &tmp_end) == true &&
		CheckDot(capsule.start, capsule.end, tmp_start) > 0.0f &&
		CheckDot(capsule.end, capsule.start, tmp_start) > 0.0f) {
		Q1inCld = true;	//Q1は円柱面にある
	}
	else {
		return false;	//レイは衝突していない
	}


	//Q2の検査
	Vector3 t;//ダミー
	if (Q1inP1 && CheckDot(capsule.end, capsule.start, tmp_end) <= 0.0f) {
		//Q1、Q2共に球p!上にある
	}
	else if (Q1inP2 && CheckDot(capsule.start, capsule.end, tmp_end) <= 0.0f) {
		//Q1、Q2共に球p2上にある
	}
	else if (Q1inCld && 
		CheckDot(capsule.start, capsule.end, tmp_end) > 0.0f &&
		CheckDot(capsule.end, capsule.start, tmp_end) > 0.0f) {
		//Q1、Q2共に球円柱面にある
	}
	else if (CalcInterRay2Sphere(ray, p1, &t, &tmp_end) == true &&
		CheckDot(capsule.end, capsule.start, tmp_end) <= 0.0f) {
		//Q2は球P1上にある
	}
	else if (CalcInterRay2Sphere(ray, p2, &t, &tmp_end) == true &&
		CheckDot(capsule.start, capsule.end, tmp_end) <= 0.0f) {
		//Q2は球P2上にある
	}

	//Q2が円柱上にある事が確定
	CalcInterRay2InfCilinder(ray, capsule.start, capsule.end, capsule.radius, &t, &tmp_end);

	if (distance) {
		*distance = (tmp_start - ray.start).Length();
	}
	if (start) {
		*start = tmp_start;
	}
	if (end) {
		*end = tmp_end;
	}
	if (normal) {
		if (Q1inP1) {
			*normal = (tmp_start - capsule.start).Normalize();
		}
		else if (Q1inP2) {
			*normal = (tmp_start - capsule.end).Normalize();
		}
		else if (Q1inCld) {
			Vector3 v1 = capsule.end - capsule.start;
			Vector3 v2 = tmp_start - capsule.start;
			Vector3 cross = v1.Cross(v2);
			Vector3 invNormal = v1.Cross(cross);
			*normal = -invNormal.Normalize();

		}
	}

	return true;
}

float Collision::CheckDot(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	return (p1.x - p2.x) * (p3.x - p2.x) + (p1.y - p2.y) * (p3.y - p2.y) + (p1.z - p2.z) * (p3.z - p2.z);
}

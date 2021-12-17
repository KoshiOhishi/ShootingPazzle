#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4.h"
#include <cmath>

//Todo:

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::Quaternion(const Vector3 & v, float angle)
{
	float _sin = sin(angle / 2.0f);
	*this = Quaternion(_sin * v.x, _sin * v.y, _sin * v.z, cos(angle / 2.0f));
}

Quaternion::Quaternion(const Vector3 & v, const Quaternion& q)
{
	Quaternion qq = q.Conjugate();	//q の共役四元数を作成する
	Quaternion qv(v.x, v.y, v.z, 0);	//ベクトル v をクォータニオン化する

	*this = q * qv * qq;	//ベクトルを回転したクォータニオンを返す
}

Quaternion::Quaternion(const Matrix4& m)
{
	Quaternion result;

	float tr = m.r[0].m128_f32[0] + m.r[1].m128_f32[1] + m.r[2].m128_f32[2] + m.r[3].m128_f32[3];
	if (tr >= 1.0f) {
		float fourD = 2.0f * sqrt(tr);
		result.x = (m.r[1].m128_f32[2] - m.r[2].m128_f32[1]) / fourD;
		result.y = (m.r[2].m128_f32[0] - m.r[0].m128_f32[2]) / fourD;
		result.z = (m.r[0].m128_f32[1] - m.r[1].m128_f32[0]) / fourD;
		result.w = fourD / 4.0f;
		*this = result;
	}
	int i = 0;
	if (m.r[0].m128_f32[0] <= m.r[1].m128_f32[1]) {
		i = 1;
	}
	if (m.r[2].m128_f32[2] > m.r[i].m128_f32[i]) {
		i = 2;
	}
	int j = (i + 1) % 3;
	int k = (j + 1) % 3;
	tr = m.r[i].m128_f32[i] - m.r[j].m128_f32[j] - m.r[k].m128_f32[k] + 1.0f;
	float fourD = 2.0f * sqrt(tr);
	float qa[4];
	qa[i] = fourD / 4.0f;
	qa[j] = (m.r[j].m128_f32[i] + m.r[i].m128_f32[j]) / fourD;
	qa[k] = (m.r[k].m128_f32[i] + m.r[i].m128_f32[k]) / fourD;
	qa[3] = (m.r[j].m128_f32[k] - m.r[k].m128_f32[j]) / fourD;
	result.x = qa[0];
	result.y = qa[1];
	result.z = qa[2];
	result.w = qa[3];

	*this = result;
}

const Quaternion& Quaternion::CreateQuaternion(float x, float y, float z, float w)
{
	return Quaternion(x, y, z, w);
}

const Quaternion& Quaternion::CreateQuaternion(const Vector3& v, float angle)
{
	return Quaternion(v, angle);
}

const Quaternion& Quaternion::CreateQuaternion(const Vector3& v, const Quaternion& q)
{
	return Quaternion(v, q);
}

const Quaternion& Quaternion::CreateQuaternion(const Matrix4& m)
{
	return Quaternion(m);
}

const float Quaternion::Dot(const Quaternion& q)const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

const float Quaternion::Length()const
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
}

const Quaternion& Quaternion::Normalize()const
{
	Quaternion result = *this;
	float len = this->Length();
	if (len != 0) {
		result /= len;
	}
	return result;
}

const Quaternion& Quaternion::Conjugate()const
{
	Quaternion result{ -x,-y,-z,w };
	return result;
}

const Quaternion& Quaternion::SLerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = q1.Dot(q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	if ((1.0f - cos) > 0.001f) {
		float theta = (float)acos(cos);
		k0 = (float)(sin(theta * k0) / sin(theta));
		k1 = (float)(sin(theta * k1) / sin(theta));
	}
	return q1 * k0 + t2 * k1;
}

const Quaternion& Quaternion::Lerp(const Quaternion & q1, const Quaternion & q2, float t)
{
	float cos = q1.Dot(q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	return q1 * k0 + t2 * k1;
}

const Matrix4& Quaternion::Rotate(const Quaternion & q)
{
	float xx = q.x * q.x * 2.0f;
	float yy = q.y * q.y * 2.0f;
	float zz = q.z * q.z * 2.0f;
	float xy = q.x * q.y * 2.0f;
	float xz = q.x * q.z * 2.0f;
	float yz = q.y * q.z * 2.0f;
	float wx = q.w * q.x * 2.0f;
	float wy = q.w * q.y * 2.0f;
	float wz = q.w * q.z * 2.0f;
	Matrix4 result = {
		1.0f - yy - zz, xy + wz, xz - wy, 0.0f,
		xy - wz, 1.0f - xx - zz, yz + wx, 0.0f,
		xz + wy, yz - wx, 1.0f - xx - yy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

const Vector3& Quaternion::GetAxis(const Quaternion & q)
{
	Vector3 result;

	float x = q.x;
	float y = q.y;
	float z = q.z;
	float _len = q.Length();

	result.x = x / _len;
	result.y = y / _len;
	result.z = z / _len;

	return result;
}



//単項演算子 + のオーバーロード
//単項演算子 + は特に意味を持たない
//（+クォータニオン）
Quaternion operator+(const Quaternion& q)
{
	return q;
}

//単項演算子 - のオーバーロード
//（-クォータニオン）
Quaternion operator-(const Quaternion& q)
{
	Quaternion result{ -q.x, -q.y, -q.z, -q.w };
	return result;
}

//代入演算子 += のオーバーロード
//（クォータニオン += クォータニオン）
Quaternion& operator+=(Quaternion& q1, const Quaternion& q2)
{
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.w += q2.w;
	return q1;
}

//代入演算子 -= のオーバーロード
//（クォータニオン -= クォータニオン）
Quaternion& operator-=(Quaternion& q1, const Quaternion& q2)
{
	q1.x -= q2.x;
	q1.y -= q2.y;
	q1.z -= q2.z;
	q1.w -= q2.w;
	return q1;
}

//代入演算子 *= のオーバーロード
//（クォータニオン *= スカラー）
Quaternion& operator*=(Quaternion& q, float s)
{
	Quaternion result{
		q.x * s, q.y * s, q.z * s, q.w * s
	};
	q = result;
	return q;
}

Quaternion& operator/=(Quaternion& q, float s)
{
	return q *= 1.0f / s;
}

//代入演算子 *= のオーバーロード
//（クォータニオン *= クォータニオン）
Quaternion& operator*=(Quaternion& q1, const Quaternion& q2)
{
	Quaternion result{
		 q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
		-q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
		 q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
		-q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w
	};
	q1 = result;
	return q1;
}

//2項演算子 + のオーバーロード
//（クォータニオン + クォータニオン）
Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result += q2;
}

//2項演算子 - のオーバーロード
//（クォータニオン - クォータニオン）
Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result -= q2;
}

//2項演算子 * のオーバーロード
//（クォータニオン * クォータニオン）
Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result *= q2;
}

//2項演算子 * のオーバーロード
//（クォータニオン * スカラー）
Quaternion operator*(const Quaternion& q, float s)
{
	Quaternion result = q;
	return result *= s;
}

//2項演算子 * のオーバーロード
//（スカラー * クォータニオン）
Quaternion operator*(float s, const Quaternion& q)
{
	Quaternion result = q;
	return result *= s;
}

//2項演算子 / のオーバーロード
//（クォータニオン / スカラー）
Quaternion operator/(const Quaternion& q, float s)
{
	Quaternion result = q;
	return result /= s;
}

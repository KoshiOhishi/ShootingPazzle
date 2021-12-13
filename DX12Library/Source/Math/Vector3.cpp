#include "Vector3.h"
#include <cmath>	// sqrt

Vector3::Vector3()
{
	x = y = z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vector3::Length() const
{
	return sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSq() const
{
	return (x * x + y * y + z * z);
}

Vector3& Vector3::Normalize() const
{
	float len = Length();
	Vector3 temp(this->x, this->y, this->z);
	if (len != 0)
	{
		return temp /= len;
	}
	return temp;
}

float Vector3::Dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3& Vector3::Cross(const Vector3& v) const
{
	Vector3 temp(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	return temp;
}

Vector3& Vector3::Zero()
{
	return Vector3();
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}
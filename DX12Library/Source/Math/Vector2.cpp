#include "Vector2.h"
#include <cmath>	// sqrt

Vector2::Vector2()
{
	x = y = 0;
}

Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

const float Vector2::Length() const
{
	return sqrt(x * x + y * y);
}

const Vector2 Vector2::Normalize() const
{
	float len = Length();
	Vector2 temp(this->x, this->y);
	if (len != 0)
	{
		return temp /= len;
	}
	return temp;
}

const float Vector2::Dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

const float Vector2::Cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

const Vector2 Vector2::Zero()
{
	return Vector2();
}

Vector2 Vector2::operator+() const
{
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator*(float s, const Vector2& v)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp /= s;
}

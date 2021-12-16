#pragma once
#include <DirectXMath.h>

struct Vector4 : public DirectX::XMFLOAT4
{
	//�R���X�g���N�^
	Vector4();								//��x�N�g���Ƃ���
	Vector4(float x, float y, float z, float w);		//x�����Ay�����Az�������w�肵�Ă̐���

	/// <summary>
	/// ��x�N�g����Ԃ�
	/// </summary>
	/// <returns></returns>
	const Vector4& Zero() const;

	//�P�����Z�q�I�[�o�[���[�h
	Vector4 operator+() const;
	Vector4 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�����Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);


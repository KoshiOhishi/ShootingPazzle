#pragma once
#include <DirectXMath.h>

struct Vector3 : public DirectX::XMFLOAT3
{

	//�R���X�g���N�^
	Vector3();								//��x�N�g���Ƃ���
	Vector3(float x, float y, float z);		//x�����Ay�����Az�������w�肵�Ă̐���

	//�����o�֐�

	/// <summary>
	/// �m���������߂�
	/// </summary>
	/// <returns>�m����(����)</returns>
	const float Length() const;

	/// <summary>
	/// �m������2���Ԃ� (�����蔻��p)
	/// </summary>
	/// <returns></returns>
	const float LengthSq() const;

	/// <summary>
	/// ���K�������x�N�g����Ԃ�
	/// </summary>
	/// <returns>���K�������x�N�g��</returns>
	const Vector3 Normalize() const;

	/// <summary>
	/// ���ς����߂�
	/// </summary>
	/// <param name="v">�x�N�g��</param>
	/// <returns>����</returns>
	const float Dot(const Vector3& v) const;

	/// <summary>
	/// �O�ς����߂�
	/// </summary>
	/// <param name="v">�x�N�g��</param>
	/// <returns>�O��</returns>
	const Vector3 Cross(const Vector3& v) const;

	/// <summary>
	/// ��x�N�g�����擾
	/// </summary>
	/// <returns></returns>
	static const Vector3 Zero();

	//�P�����Z�q�I�[�o�[���[�h
	Vector3 operator+() const;
	Vector3 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�����Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);



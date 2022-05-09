#pragma once
#include <DirectXMath.h>

namespace DX12Library {

	struct Vector2 : public DirectX::XMFLOAT2
	{
		//�R���X�g���N�^
		Vector2();								//��x�N�g���Ƃ���
		Vector2(float x, float y);		//x�����Ay�������w�肵�Ă̐���

			//�����o�֐�

		/// <summary>
		/// �m���������߂�
		/// </summary>
		/// <returns>�m����(����)</returns>
		const float Length() const;

		/// <summary>
		/// ���K�������x�N�g����Ԃ�
		/// </summary>
		/// <returns>���K�������x�N�g��</returns>
		const Vector2 Normalize() const;

		/// <summary>
		/// ���ς����߂�
		/// </summary>
		/// <param name="v">�x�N�g��</param>
		/// <returns>����</returns>
		const float Dot(const Vector2& v) const;

		/// <summary>
		/// �O�ς����߂�
		/// </summary>
		/// <param name="v">�x�N�g��</param>
		/// <returns>�O��</returns>
		const float Cross(const Vector2& v) const;

		/// <summary>
		/// ��x�N�g�����擾
		/// </summary>
		/// <returns></returns>
		static const Vector2 Zero();

		//�P�����Z�q�I�[�o�[���[�h
		Vector2 operator+() const;
		Vector2 operator-() const;

		//������Z�q�I�[�o�[���[�h
		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator*=(float s);
		Vector2& operator/=(float s);
	};
}

using Vector2 = DX12Library::Vector2;
//2�����Z�q�I�[�o�[���[�h
//�����Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);
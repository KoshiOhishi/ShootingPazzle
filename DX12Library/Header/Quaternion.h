#pragma once
#include "Vector3.h"
#include "Matrix4.h"

namespace DX12Library {

	struct Quaternion
	{
		float x;	//x����
		float y;	//y����
		float z;	//z����
		float w;	//w����

		//�R���X�g���N�^
		Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
		Quaternion(const Vector3& v, float angle);
		Quaternion(const Vector3& v, const Quaternion& q);
		Quaternion(const Matrix4& m);

		/// <summary>
		/// �l���w�肵�č쐬�����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion CreateQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

		/// <summary>
		/// �C�ӎ��܂��̉�]�̎w�肩��쐬�����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="v">��]����x�N�g��</param>
		/// <param name="angle">�p�x(Digrees)</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion CreateQuaternion(const Vector3& v, float angle);

		/// <summary>
		/// �x�N�g������]�����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="v">��]����x�N�g��</param>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion CreateQuaternion(const Vector3& v, const Quaternion& q);

		/// <summary>
		/// ��]�s�񂩂�쐬�����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="m">��]�s��</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion CreateQuaternion(const Matrix4& m);

		/// <summary>
		/// ���ς����߂�
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>����</returns>
		const float Dot(const Quaternion& q) const;

		/// <summary>
		/// �m���������߂�
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>�m����(����)</returns>
		const float Length() const;

		/// <summary>
		/// ���K������
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>���K�������N�H�[�^�j�I��</returns>
		const Quaternion Normalize()const;

		/// <summary>
		/// �����l���������߂�
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>�����l����</returns>
		const Quaternion Conjugate()const;

		/// <summary>
		/// ���ʕ�Ԃ����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="q1">�J�n�N�H�[�^�j�I��</param>
		/// <param name="q2">�I���N�H�[�^�j�I��</param>
		/// <param name="t">�p�����[�^ (0�`1)</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float t);

		/// <summary>
		/// ���`��Ԃ����N�H�[�^�j�I����Ԃ�
		/// </summary>
		/// <param name="q1">�J�n�N�H�[�^�j�I��</param>
		/// <param name="q2">�I���N�H�[�^�j�I��</param>
		/// <param name="t">�p�����[�^ (0�`1)</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

		/// <summary>
		/// �N�H�[�^�j�I�������]�����s���Ԃ�
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>�N�H�[�^�j�I��</returns>
		static const Matrix4 Rotate(const Quaternion& q);

		/// <summary>
		/// ��]���̎Z�o
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I��</param>
		/// <returns>��]���x�N�g��</returns>
		static const Vector3 GetAxis(const Quaternion& q);
	};
}

using Quaternion = DX12Library::Quaternion;
//�P�����Z�q�I�[�o�[���[�h
Quaternion operator+(const Quaternion& q);
Quaternion operator-(const Quaternion& q);

//������Z�q�I�[�o�[���[�h
Quaternion& operator+=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator-=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator*=(Quaternion& q, float s);
Quaternion& operator/=(Quaternion& q, float s);
Quaternion& operator*=(Quaternion& q1, const Quaternion& q2);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q, float s);
Quaternion operator*(float s, const Quaternion& q);
Quaternion operator/(const Quaternion& q, float s);

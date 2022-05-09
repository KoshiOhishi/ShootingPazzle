#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace DX12Library {

	using Matrix4 = DirectX::XMMATRIX;

	//�P�ʍs������߂�
	Matrix4 Matrix4Identity();

	//�g��k���s��̐ݒ�
	Matrix4 Matrix4Scale(const Vector3& s);

	//��]�s��̐ݒ�
	Matrix4 Matrix4RotateX(float angle);
	Matrix4 Matrix4RotateY(float angle);
	Matrix4 Matrix4RotateZ(float angle);

	//���s�ړ��s��̍쐬
	Matrix4 Matrix4Translate(const Vector3& t);

	//���W�ϊ��i�x�N�g���ƍs��̊|���Z������j
	Vector3 Matrix4Transform(const Vector3& v, const Matrix4& m);

}
#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

// MATRIX �^�� Matrix4 �^�Ƃ��Ĉ��� ���݊����̊m��&�g���̂���
using Matrix4 = DirectX::XMMATRIX;

//�P�ʍs������߂�
Matrix4 Identity();

//�g��k���s��̐ݒ�
Matrix4 Scale(const Vector3& s);

//��]�s��̐ݒ�
Matrix4 RotateX(float angle);
Matrix4 RotateY(float angle);
Matrix4 RotateZ(float angle);

//���s�ړ��s��̍쐬
Matrix4 Translate(const Vector3& t);

//���W�ϊ��i�x�N�g���ƍs��̊|���Z������j
Vector3 Transform(const Vector3& v, const Matrix4& m);


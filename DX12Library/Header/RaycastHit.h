#pragma once

#include "BaseCollider.h"
#include <DirectXMath.h>
#include "Vector3.h"

class Object3D;

/// <summary>
/// ���C�L���X�g�ɂ����𓾂�ׂ̍\����
/// </summary>
struct RaycastHit
{
	// �Փˑ���̃I�u�W�F�N�g
	Object3D* object = nullptr;
	// �Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	// �Փ˓_
	Vector3 inter;
	// �Փ˓_�܂ł̋���
	float distance = 0.0f;
};


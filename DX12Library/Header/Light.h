#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "Vector3.h"
#include "Camera.h"

/// <summary>
/// ���C�g
/// </summary>
class Light
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector3 lightDir;	//���C�g�ւ̕�����\���x�N�g��
		float pad; //�p�f�B���O
		Vector3 lightColor;	//���C�g�̐F
	};


private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//���C�g�ʒu
	Vector3 lightPos = { 0,0,0 };
	//���C�g���������i�P�ʃx�N�g���j
	Vector3 lightDir = Vector3(1,-1,1).Normalize();
	//���C�g�F
	Vector3 lightColor = { 1,1,1 };
	//���C�g�����_
	Vector3 lighttarget = { 0,0,0 };
	//�_�[�e�B�t���O
	bool dirty = false;

public://�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(UINT rootParameterIndex);

	/// <summary>
	/// �萔�o�b�t�@�]��
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// ���C�g�̈ʒu�Z�o
	/// </summary>
	/// <param name="camera"></param>
	void CalcLightPos(float distance);

	/// <summary>
	/// ���C�g�̍��W���Z�b�g
	/// (��قǂ���Ȃ�����CalcLightPos���g�p���Ă�������)
	/// </summary>
	/// <param name="lightpos"></param>
	void SetLightPos(const Vector3& lightPos) { this->lightPos = lightPos; }

	/// <summary>
	/// ���C�g�̏Ɠ_�����Z�b�g
	/// </summary>
	/// <param name="lightdistance"></param>
	void SetLightTarget(const Vector3& lighttarget) { this->lighttarget = lighttarget; }

	/// <summary>
	/// ���C�g�������Z�b�g
	/// </summary>
	/// <param name="lightdir">���C�g����</param>
	void SetLightDir(const Vector3& lightDir);

	/// <summary>
	/// ���C�g�������Z�b�g
	/// </summary>
	/// <param name="dirX">���C�g����X</param>
	/// <param name="dirY">���C�g����Y</param>
	/// <param name="dirZ">���C�g����Z</param>
	void SetLightDir(const float dirX, const float dirY, const float dirZ);

	/// <summary>
	/// ���C�g�F���Z�b�g
	/// </summary>
	/// <param name="lightcolor">���C�g�F</param>
	void SetLightColor(const Vector3& lightcolor);

	/// <summary>
	/// ���C�g�F���Z�b�g (0�`1�w��o�[�W����)
	/// </summary>
	/// <param name="R">�Ԑ���</param>
	/// <param name="G">�ΐ���</param>
	/// <param name="B">����</param>
	void SetLightColor(const float R, const float G, const float B);

	/// <summary>
	/// ���C�g�F���Z�b�g (0�`255�w��o�[�W����)
	/// </summary>
	/// <param name="R">�Ԑ���</param>
	/// <param name="G">�ΐ���</param>
	/// <param name="B">����</param>
	void SetLightColorAs0To255(const float R, const float G, const float B);

	/// <summary>
	/// ���C�g�̕����擾
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightDir()const { return lightDir; }

	/// <summary>
	/// ���C�g�̈ʒu�擾
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightPos()const { return lightPos; }

	/// <summary>
	/// ���C�g�̏Ɠ_�����擾
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightTarget()const { return lighttarget; }
};


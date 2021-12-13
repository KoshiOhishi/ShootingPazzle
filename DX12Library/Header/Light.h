#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "Vector3.h"

/// <summary>
/// ���C�g
/// </summary>
class Light
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMVECTOR lightv;	//���C�g�ւ̕�����\���x�N�g��
		Vector3 lightcolor;	//���C�g�̐F
	};


private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//���C�g���������i�P�ʃx�N�g���j
	XMVECTOR lightdir = {1,0,0,0};
	//���C�g�F
	Vector3 lightcolor = { 1,1,1 };
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
	/// ���C�g�������Z�b�g
	/// </summary>
	/// <param name="lightdir">���C�g����</param>
	void SetLightDir(const XMVECTOR& lightdir);

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
};


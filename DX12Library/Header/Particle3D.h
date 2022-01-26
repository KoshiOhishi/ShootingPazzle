#pragma once
#include "ParticleManager.h"

enum ParticleBillboardType {
	BILLBOARD_TYPE_NONE,
	BILLBOARD_TYPE_XYZ,
	BILLBOARD_TYPE_Y
};

class Particle3D :
	public ParticleManager
{
public:
	//�p�[�e�B�N��1��
	struct Particle
	{
		//���W
		Vector3 position = {};

		//���x
		Vector3 velocity = {};

		//�����x
		Vector3 accel = {};

		//��������
		Timer life;

		//�X�P�[��
		float scale = 1.0f;
		//�����l
		float s_scale = 1.0f;
		//�ŏI�l
		float e_scale = 0.0f;
		//���Ԓl
		float m_scale = 0.5f;
	};

	// ���_�f�[�^�\����
	struct VertexPos3D
	{
		Vector3 pos; // xyz���W
		float scale;	//�X�P�[��
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData3D
	{
		XMMATRIX mat;	// �r���[�v���W�F�N�V�����s��
		XMMATRIX matBillboard;	//�r���{�[�h�s��
		Vector4 color; //�F
	};

private:
	//�J����
	static Camera* camera;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate[PIPELINE_COUNT];

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �J�����Z�b�g
	/// </summary>
	/// <param name="camera"></param>
	static void SetCamera(Camera* camera) { Particle3D::camera = camera; }

	/// <summary>
	/// �`��O����
	/// </summary>
	static void BeginDraw();

private:

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	static void CreateGraphicsPipeline();

public:
	virtual void Initialize() override;
	/// <summary>
	/// ���t���[������
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() override;

	virtual void DrawPrivate() override;

	/// <summary>
	/// �p�[�e�B�N���̒ǉ�
	/// </summary>
	/// <param name="life">��������</param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	/// <param name="accel">�����x</param>
	/// <param name="start_scale">�J�n���X�P�[���l</param>
	/// <param name="end_scale">�I�����X�P�[���l</param>
	void Add(double life, const Vector3& position, const Vector3& velocity,
		const Vector3& accel, float start_scale, float end_scale);
	void Add(double life, const Vector3& position, const Vector3& velocity,
		const Vector3& accel, float start_scale, float end_scale, float middle_scale);

	/// <summary>
	/// �r���{�[�h�^�C�v�̐ݒ�
	/// </summary>
	/// <param name="billboardType">�r���{�[�h�^�C�v 0...���� 1...�S���� 2...Y��</param>
	void SetBillboardType(int billboardType) { this->billboardType = billboardType; }

private:
	/// <summary>
	/// ���f���쐬
	/// </summary>
	void CreateModel();

private:
	//�r���{�[�h�^�C�v
	int billboardType = BILLBOARD_TYPE_XYZ;
	//�p�[�e�B�N���z��
	std::forward_list<Particle> particles;
	// ���_�f�[�^�z��
	VertexPos3D vertices[VERTEX_COUNT];
};
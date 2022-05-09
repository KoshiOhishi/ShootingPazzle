#pragma once
#include "ParticleManager.h"

namespace DX12Library {

	class Particle2D :
		public ParticleManager
	{
	public:
		//�p�[�e�B�N��1��
		struct Particle
		{
			//���W
			Vector2 position = {};

			//���x
			Vector2 velocity = {};

			//�����x
			Vector2 accel = {};

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
		struct VertexPos2D
		{
			Vector3 pos; // xyz���W
			Vector2 scale;	//�X�P�[��
		};

		// �萔�o�b�t�@�p�f�[�^�\����
		struct ConstBufferData2D
		{
			XMMATRIX mat;	// �r���[�v���W�F�N�V�����s��
			Vector4 color; //�F
		};

	private:
		// ���[�g�V�O�l�`��
		static ComPtr<ID3D12RootSignature> rootsignature;
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		static ComPtr<ID3D12PipelineState> pipelinestate[PIPELINE_COUNT];
		// �ˉe�ϊ��s��
		static DirectX::XMMATRIX matProjection;

	public:
		/// <summary>
		/// �ÓI������
		/// </summary>
		static void StaticInitialize();

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
		void Add(double life, const Vector2& position, const Vector2& velocity,
			const Vector2& accel, float start_scale, float end_scale);
		void Add(double life, const Vector2& position, const Vector2& velocity,
			const Vector2& accel, float start_scale, float end_scale, float middle_scale);

	private:
		/// <summary>
		/// ���f���쐬
		/// </summary>
		void CreateModel();

	private:
		//�p�[�e�B�N���z��
		std::forward_list<Particle> particles;
		// ���_�f�[�^�z��
		VertexPos2D vertices[VERTEX_COUNT];
	};
}
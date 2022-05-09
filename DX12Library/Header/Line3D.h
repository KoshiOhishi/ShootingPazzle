#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "Vector3.h"
#include "Camera.h"

namespace DX12Library {

	struct LineInfo {
		Vector3 startPos;
		Vector3 endPos;
		Vector4 startColor;
		Vector4 endColor;
	};

	class Line3D
	{
		struct VertexLine3D {
			Vector3 pos;
			Vector4 color;
		};

		struct ConstBuffData {
			Vector4 color;
			DirectX::XMMATRIX matWorld;
		};

	private:
		static const int MAX_DRAW_COUNT = 4096;

	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		//�f�o�C�X
		static ID3D12Device* device;
		//�J����
		static Camera* camera;
		//���[�g�V�O�l�`��
		static ComPtr<ID3D12RootSignature> rootsignature;
		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
		static ComPtr<ID3D12PipelineState> pipelinestate;
		//�萔�o�b�t�@
		static ComPtr<ID3D12Resource> constBuff;
		//���_�o�b�t�@
		static ComPtr<ID3D12Resource> vertBuff;
		//�C���f�b�N�X�o�b�t�@
		static ComPtr<ID3D12Resource> indexBuff;

	private:
		static std::vector<LineInfo> lines;

	public:
		/// <summary>
		/// �`��p�B���֐��@DrawLine���g�p���Ă�������
		/// </summary>
		/// <param name="drawInfo"></param>
		static void DrawAll();

		static void SetCamera(Camera* camera) { Line3D::camera = camera; }

	public:
		/// <summary>
		/// �v���O�����J�n���ɌĂԊ֐�
		/// </summary>
		static void StaticInitialize();

		/// <summary>
		/// ���`�惊�X�g����ɂ���B�`�惋�[�v�����ɒu���B
		/// </summary>
		static void Reset();

		/// <summary>
		/// �`��O�֐�
		/// </summary>
		static void BeginDraw();

		/// <summary>
		/// 3D��Ԃɐ���`�悷��
		/// </summary>
		/// <param name="start">�n�_</param>
		/// <param name="end">�I�_</param>
		static void DrawLine(const Vector3& start, const Vector3& end);

		/// <summary>
		/// 3D��Ԃɐ���`�悷��
		/// </summary>
		/// <param name="startPos">�n�_</param>
		/// <param name="endPos">�I�_</param>
		/// <param name="color">�F</param>
		static void DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& color);

		/// <summary>
		/// 3D��Ԃɐ���`�悷��
		/// </summary>
		/// <param name="startPos">�n�_</param>
		/// <param name="endPos">�I�_</param>
		/// <param name="startColor">�n�_�̐F</param>
		/// <param name="endColor">�I�_�̐F</param>
		static void DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& startColor, const Vector4& endColor);
	};

}
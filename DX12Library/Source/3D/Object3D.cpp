#include "Object3D.h"
#include "DX12Util.h"
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "PostEffect.h"
#include "DrawManager.h"
#include "Archive.h"
#include "Encorder.h"
#include <typeinfo>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DX12Library;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Object3D::device = nullptr;
Camera* Object3D::camera = nullptr;
Light* Object3D::light = nullptr;
ComPtr <ID3D12DescriptorHeap> Object3D::descHeapSRV = nullptr;
ComPtr <ID3D12DescriptorHeap> Object3D::descHeapDepthSRV = nullptr;
ComPtr<ID3D12RootSignature> Object3D::fbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::fbxPipelinestate[PIPELINE_COUNT] = {};
ComPtr<ID3D12RootSignature> Object3D::shadowFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::shadowFbxPipelinestate = {};
ComPtr<ID3D12RootSignature> Object3D::objRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::objPipelinestate[PIPELINE_COUNT] = {};
ComPtr<ID3D12RootSignature> Object3D::shadowObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::shadowObjPipelinestate = {};
int Object3D::prevDrawObjectType = -1;
int Object3D::prevPipelineIndex = -1;
std::vector<Object3D*> Object3DManager::pObject3DList;
int Object3D::loadCount = 0;
XMMATRIX Object3D::matOrthographicLH = XMMatrixOrthographicLH(1280 * 0.5f, 720 * 0.5f, 1, 100);

void Object3D::StaticInitialize()
{
	HRESULT result;

	//SRV�p�f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	descHeapDesc.NumDescriptors = 1024;//�e�N�X�`������
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(descHeapSRV.ReleaseAndGetAddressOf()));//����

	assert(SUCCEEDED(result));

	//�[�x�e�N�X�`���pSRV�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descHeapSRV->GetCPUDescriptorHandleForHeapStart();

	DX12Util::GetDevice()->CreateShaderResourceView(DX12Util::GetDepthBuffer(1), &resDesc, handle);
	loadCount++;
}

void Object3D::CreateGraphicsPipeline(int objectType, PipelineData& pipelineData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gsBlob;	//�W�I���g���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	assert(device);

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.vertexShaderFileName), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.vertexShaderFileName));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // �C���N���[�h�\�ɂ���
				"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			pipelineData.vertexShaderFileName,    // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	if (pipelineData.geometryShaderFileName != nullptr) {
		bool isLoadedArchiveGS = false;
		if (Archive::IsOpenArchive()) {
			int size;
			void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.geometryShaderFileName), &size);
			std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.geometryShaderFileName));

			if (pData != nullptr) {

				result = D3DCompile(
					mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
					nullptr,
					nullptr, // �C���N���[�h�\�ɂ���
					"main", "gs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
					0,
					&gsBlob, &errorBlob);

				if (result == S_OK) {
					isLoadedArchiveGS = true;
				}
			}
		}

		if (isLoadedArchiveGS == false) {
			result = D3DCompileFromFile(
				pipelineData.geometryShaderFileName,    // �V�F�[�_�t�@�C����
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
				"main", "gs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
				0,
				&gsBlob, &errorBlob);
		}

		if (FAILED(result)) {
			// errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchivePS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.pixelShaderFileName), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.pixelShaderFileName));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // �C���N���[�h�\�ɂ���
				"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
				0,
				&psBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchivePS = true;
			}
		}
	}

	if (isLoadedArchivePS == false) {
		result = D3DCompileFromFile(
			pipelineData.pixelShaderFileName,    // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&psBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//// ���_���C�A�E�g
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//	{ // xy���W(1�s�ŏ������ق������₷��)
	//		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
	//		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv���W(1�s�ŏ������ق������₷��)
	//		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv���W(1�s�ŏ������ق������₷��)
	//		"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv���W(1�s�ŏ������ق������₷��)
	//		"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	if (pipelineData.geometryShaderFileName != nullptr) {
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	}
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	//���������̐[�x�l�������ݐݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;

	// �f�v�X�X�e���V���X�e�[�g
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2;    // �`��Ώۂ�3��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA

	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2];
	//�ʏ�p
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].MinLOD = 0;
	samplerDesc[0].MipLODBias = 0.0f;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;

	//�e�p
	samplerDesc[1] = samplerDesc[0];
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//��r���ʂ��o�C���j�A���
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc[1].ShaderRegister = 1;	//t1

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(pipelineData.rootparams.size(), pipelineData.rootparams.data(), _countof(samplerDesc), samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	if (objectType == ObjectType::OBJECTTYPE_OBJ) {
		// ���[�g�V�O�l�`���̐���
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(objRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = objRootsignature.Get();
	}
	else if (objectType == ObjectType::OBJECTTYPE_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(fbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = fbxRootsignature.Get();
	}

	for (int i = 0; i < PIPELINE_COUNT; i++) {
		//�u�����h�X�e�[�g�̐ݒ�
		// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(8 ���邪���܂͈�����g��Ȃ�)
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // �W���ݒ�

		blenddesc.BlendEnable = true; // �u�����h��L���ɂ���

		blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // ���Z

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // ���Z
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // �\�[�X�̒l�� 100% �g��
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // �f�X�g�̒l�� 0% �g��

		switch (i) {
		case OBJECT3D_BLENDMODE_NORMAL:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case OBJECT3D_BLENDMODE_ADD:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case OBJECT3D_BLENDMODE_SUB:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		case OBJECT3D_BLENDMODE_MUL:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case OBJECT3D_BLENDMODE_SCREEN:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case OBJECT3D_BLENDMODE_REVERSE:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}

		// �u�����h�X�e�[�g�ɐݒ肷��
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		gpipeline.BlendState.RenderTarget[1] = blenddesc;

		if (objectType == ObjectType::OBJECTTYPE_OBJ) {
			// �O���t�B�b�N�X�p�C�v���C���̐���
			result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(objPipelinestate[i].ReleaseAndGetAddressOf()));
			if (FAILED(result)) { assert(0); }
		}
		else if (objectType == ObjectType::OBJECTTYPE_FBX) {
			// �O���t�B�b�N�X�p�C�v���C���̐���
			result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(fbxPipelinestate[i].ReleaseAndGetAddressOf()));
			if (FAILED(result)) { assert(0); }
		}
	}

}

void Object3D::CreateShadowObjGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//�e�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/OBJShadowTestVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/OBJShadowTestVS.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // �C���N���[�h�\�ɂ���
				"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			L"Shader/OBJShadowTestVS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);
	}


	if (FAILED(result)) {
		// errorBlob ����G���[���e�� string �^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//���_���C�A�E�g�z��̐錾�ƁA�e�퍀�ڂ̐ݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\���̂�p�ӂ���B
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;

	//�T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	//���X�^���C�Y�X�e�[�g
	//�W���I�Ȑݒ�(�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//��U�W���l���Z�b�g
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//�J�����O���Ȃ�

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//���������̐[�x�l�������ݐݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;


	//���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`����O�p�`�ɐݒ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	//���̑��̐ݒ�
	gpipeline.NumRenderTargets = 0; // �`��Ώۂ� 1 ��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // 0~255 �w��� RGBA

	gpipeline.SampleDesc.Count = 1; // 1 �s�N�Z���ɂ� 1 ��T���v�����O

	//�f�X�N���v�^�e�[�u���̐ݒ�
	//�f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 ���W�X�^

	//���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[2];
	//�萔�p
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0)
	//�e�N�X�`���p
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//�T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	//samplerDesc.MaxAnisotropy = 1;

	//���[�g�V�O�l�`���̐���
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr <ID3DBlob> rootSigBlob;
	//�o�[�W������������ł̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//���[�g�V�O�l�`���̐���
	result = DX12Util::GetDevice()->CreateRootSignature(0, rootSigBlob.Get()->GetBufferPointer(), rootSigBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(shadowObjRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = shadowObjRootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowObjPipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
	
}

void Object3D::CreateShadowFbxGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//�e�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/FBXShadowTestVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/FBXShadowTestVS.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // �C���N���[�h�\�ɂ���
				"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			L"Shader/FBXShadowTestVS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob ����G���[���e�� string �^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//���_���C�A�E�g�z��̐錾�ƁA�e�퍀�ڂ̐ݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �X�L���C���f�b�N�X
			"BONEINDICES",
			0, 
			DXGI_FORMAT_R32G32B32A32_UINT, 
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,

			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
			0
		},
		{ // �X�L���E�F�C�g
			"BONEWEIGHTS", 
			0, 
			DXGI_FORMAT_R32G32B32A32_FLOAT, 
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
			0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\���̂�p�ӂ���B
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;

	//�T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	//���X�^���C�Y�X�e�[�g
	//�W���I�Ȑݒ�(�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//��U�W���l���Z�b�g
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//�J�����O���Ȃ�

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//���������̐[�x�l�������ݐݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;

	//���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`����O�p�`�ɐݒ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	//���̑��̐ݒ�
	gpipeline.NumRenderTargets = 0; // �`��Ώۂ� 1 ��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // 0~255 �w��� RGBA

	gpipeline.SampleDesc.Count = 1; // 1 �s�N�Z���ɂ� 1 ��T���v�����O

	//�f�X�N���v�^�e�[�u���̐ݒ�
	//�f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 ���W�X�^

	//���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[3];
	//�萔�p
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0)
	//�e�N�X�`���p
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//FBX�X�L�j���O
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);


	//�T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	//samplerDesc.MaxAnisotropy = 1;

	//���[�g�V�O�l�`���̐���
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr <ID3DBlob> rootSigBlob;
	//�o�[�W������������ł̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//���[�g�V�O�l�`���̐���
	result = DX12Util::GetDevice()->CreateRootSignature(0, rootSigBlob.Get()->GetBufferPointer(), rootSigBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(shadowFbxRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = shadowFbxRootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowFbxPipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
}

Object3D::~Object3D()
{
	if (collider) {
		//�R���W�����}�l�[�W������o�^����������
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

void Object3D::Initialize()
{
	HRESULT result;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataShare) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffShare.ReleaseAndGetAddressOf())
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffTransform.ReleaseAndGetAddressOf())
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataShadow) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffShadow.ReleaseAndGetAddressOf())
	);
	
	////�I�u�W�F�N�g�}�l�[�W���[�ɒǉ�
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//�N���X���̕�������擾
	name = typeid(*this).name();
}

void Object3D::Update()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//���[���h�s��̍���
	matWorld = XMMatrixIdentity();	//�ό`�����Z�b�g
	matWorld *= matScale;			//���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot;				//���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans;			//���[���h�s��ɕ��s�ړ��𔽉f

	//�r���[�v���W�F�N�V�����s��
	XMMATRIX matViewProjection = camera->GetViewProjection();

	//���C�g���
	Vector3 target = light->GetLightTarget();
	Vector3 up = { 0,1,0 };

	//���C�g�̃r���[�s��
	XMMATRIX matLight = XMMatrixIdentity();
	XMMATRIX matLightView = XMMatrixLookAtLH(
		XMLoadFloat3(&light->GetLightPos()), XMLoadFloat3(&target), XMLoadFloat3(&up)
	);
	//���s���e
	XMMATRIX matLightProj = matOrthographicLH;

	//�s�������
	matLight = matLightView * matLightProj;


	if (objectType == OBJECTTYPE_OBJ) {

		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->isDrawShadowToMyself = isDrawShadowToMyself;
			constMap->cameraPos = camera->GetPosition();
			constBuffShare->Unmap(0, nullptr);
		}

		ConstBufferDataTransform* constMap2 = nullptr;
		result = constBuffTransform->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {
			constMap2->data.color = color;
			constMap2->data.world = matWorld;
			constBuffTransform->Unmap(0, nullptr);
		}
	}
	else if (objectType == OBJECTTYPE_FBX) {

		//���f���̃��b�V���g�����X�t�H�[��
		XMMATRIX modelTransform = fbxModel->GetModelTransForm();


		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->cameraPos = camera->GetPosition();
			constBuffShare->Unmap(0, nullptr);
		}

		ConstBufferDataTransform* constMap2 = nullptr;
		result = constBuffTransform->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {
			constMap2->data.color = color;
			constMap2->data.world = modelTransform * matWorld;
			constBuffTransform->Unmap(0, nullptr);
		}

		//�A�j���[�V����
		if (isPlay) {
			//1�t���[���i�߂�
			currentTime += frameTime;
			//�Ō�܂ōĐ�������擪�ɖ߂�
			if (currentTime > endTime) {
				currentTime = startTime;
			}
		}

		//�{�[���z��
		std::vector<FbxModel::Bone>& bones = fbxModel->GetBones();

		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataSkin* constMapSkin = nullptr;
		result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
		for (int i = 0; i < bones.size(); i++) {
			//���̎p���s��
			XMMATRIX matCurrentPose;
			//���̎p���s����擾
			FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			//XMMATRIX�ɕϊ�
			FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
			//�������ăX�L�j���O�s���
			constMapSkin->bones[i] = fbxModel->GetModelTransForm() * bones[i].invInitialPose * matCurrentPose * fbxModel->GetModelTransForm();
		}
		constBuffSkin->Unmap(0, nullptr);

	}

	ConstBufferDataShadow* constMap3 = nullptr;
	result = constBuffShadow->Map(0, nullptr, (void**)&constMap3);
	if (SUCCEEDED(result)) {
		constMap3->lightCamera = matLight;
		constMap3->world = matWorld;
		constBuffShadow->Unmap(0, nullptr);
	}


	//�����蔻��X�V
	if (collider) {
		collider->Update();
	}
}

void Object3D::BeginDraw()
{
	//�f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//�v���~�e�B�u�`���ݒ�
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	prevDrawObjectType = -1;
	prevPipelineIndex = -1;
}

void Object3D::DrawPrivate()
{
	ConstBufferDataTransform* constMap = nullptr;

	if (objectType == OBJECTTYPE_OBJ) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (objModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//���[�g�V�O�l�`���̐ݒ�
			DX12Util::GetCmdList()->SetGraphicsRootSignature(objRootsignature.Get());
			//�p�C�v���C���X�e�[�g�̐ݒ�
			DX12Util::GetCmdList()->SetPipelineState(objPipelinestate[pipelineIndex].Get());
		}
		//�O��̕`��Ɏg�p���ꂽ�p�C�v���C���̃C���f�b�N�X�ƍ���g�����̂��Ⴄ�Ȃ�
		//�����Ő؂�ւ�
		else if (pipelineIndex != prevPipelineIndex) {
			//�p�C�v���C���X�e�[�g�̐ݒ�
			DX12Util::GetCmdList()->SetPipelineState(objPipelinestate[pipelineIndex].Get());
		}
		prevPipelineIndex = pipelineIndex;

		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		// �V�F�[�_���\�[�X�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(5,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
				0,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
		//���C�g�̕`��
		light->Draw(3);
		//���f���`��
		if (objModel != nullptr) {
			objModel->Draw();
		}

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (fbxModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//���[�g�V�O�l�`���̐ݒ�
			DX12Util::GetCmdList()->SetGraphicsRootSignature(fbxRootsignature.Get());
		}

		//�O��̕`��Ɏg�p���ꂽ�p�C�v���C���̃C���f�b�N�X�ƍ���g�����̂��Ⴄ�Ȃ�
		//�����Ő؂�ւ�
		static int prevPipelineIndex = -1;
		if (pipelineIndex != prevPipelineIndex) {
			//�p�C�v���C���X�e�[�g�̐ݒ�
			DX12Util::GetCmdList()->SetPipelineState(fbxPipelinestate[pipelineIndex].Get());
		}
		prevPipelineIndex = pipelineIndex;

		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
		// �V�F�[�_���\�[�X�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(7,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
				0,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
		//���C�g�̕`��
		light->Draw(3);
		//���f���`��
		fbxModel->Draw();
	}

	prevDrawObjectType = objectType;
}

void Object3D::BeginDrawShadow()
{
	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = DX12Util::GetDepthHeap()->GetCPUDescriptorHandleForHeapStart();
	dsvH.ptr += DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//�����_�[�^�[�Q�b�g���Z�b�g
	DX12Util::GetCmdList()->OMSetRenderTargets(0, nullptr, false, &dsvH);

	//4.�r���[�|�[�g�̐ݒ�R�}���h
	DX12Util::GetCmdList()->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight()));
	DX12Util::GetCmdList()->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight()));

	//5.�V�U�[��`�̐ݒ�R�}���h
	D3D12_RECT scissorrect{};
	scissorrect.left = 0; // �؂蔲�����W��
	scissorrect.right = scissorrect.left + DX12Util::GetWindowWidth(); // �؂蔲�����W�E
	scissorrect.top = 0; // �؂蔲�����W��
	scissorrect.bottom = scissorrect.top + DX12Util::GetWindowHeight(); // �؂蔲�����W��
	DX12Util::GetCmdList()->RSSetScissorRects(1, &scissorrect);

	////�f�X�N���v�^�q�[�v�̃Z�b�g
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeapDepthSRV.Get() };
	//DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//�v���~�e�B�u�`���ݒ�
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�w�i�`�悱���܂�
	DX12Util::ClearDepthBuffer(true);

	prevDrawObjectType = -1;
	prevPipelineIndex = -1;
}

void Object3D::DrawShadow()
{
	if (isDrawShadowToOther == false) {
		return;
	}

	if (objectType == OBJECTTYPE_OBJ) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (objModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//���[�g�V�O�l�`���̐ݒ�
			DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowObjRootsignature.Get());

			//�p�C�v���C���X�e�[�g�̐ݒ�
			DX12Util::GetCmdList()->SetPipelineState(shadowObjPipelinestate.Get());
		}
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadow->GetGPUVirtualAddress());

		//���f���`��
		if (objModel != nullptr) {
			objModel->Draw(1, true);
		}

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (fbxModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//���[�g�V�O�l�`���̐ݒ�
			DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowFbxRootsignature.Get());

			//�p�C�v���C���X�e�[�g�̐ݒ�
			DX12Util::GetCmdList()->SetPipelineState(shadowFbxPipelinestate.Get());
		}
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadow->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());
		//���f���`��
		fbxModel->Draw(1, true);
	}
}

void Object3D::Draw()
{
	DrawManager::AddDrawList(DRAW_MANAGER_OBJECT_TYPE_OBJECT3D, this);
}

void Object3D::PlayAnimation()
{
	if (objectType == OBJECTTYPE_OBJ || objectType == OBJECTTYPE_INSTANCING_OBJ) {
		return;
	}

	FbxScene* fbxScene = fbxModel->GetFbxScene();
	//0�Ԃ̃A�j���[�V�����擾
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//�A�j���[�V�����̖��O�擾
	const char* animstackname = animstack->GetName();
	//�A�j���[�V�����̎��Ԏ擾
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//�J�n���Ԏ擾
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	//�I�����Ԏ擾
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	//�J�n���Ԃɍ��킹��
	currentTime = startTime;
	//�Đ�����Ԃɂ���
	isPlay = true;
}

void Object3D::SetFbxModel(FbxModel* fbxModel)
{
	this->fbxModel = fbxModel;

	//�萔�o�b�t�@�̐���
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffSkin.ReleaseAndGetAddressOf())
	);

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < MAX_BONES; i++) {
			constMapSkin->bones[i] = XMMatrixIdentity();
		}
		constBuffSkin->Unmap(0, nullptr);
	}

	//1�t���[�����̎��Ԃ�60FPS�Őݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	objectType = OBJECTTYPE_FBX;
}

void Object3D::SetObjModel(ObjModel* objModel)
{
	this->objModel = objModel;

	objectType = OBJECTTYPE_OBJ;
}

void Object3D::SetCollider(BaseCollider* collider)
{
	//���ɓo�^����Ă���ꍇ��Remove
	if (this->collider) {
		CollisionManager::GetInstance()->RemoveCollider(this->collider);
	}

	collider->SetObject(this);
	this->collider = collider;
	//�R���W�����}�l�[�W���ɓo�^
	CollisionManager::GetInstance()->AddCollider(collider);
	//�R���C�_�[���X�V���Ă���
	collider->Update();
}

void Object3DManager::Initialize()
{
	pObject3DList.clear();
}

void Object3DManager::AddObject(Object3D* p)
{
	pObject3DList.emplace_back(p);
}

void Object3DManager::UpdateAllObject()
{
	for (int i = 0; i < pObject3DList.size(); i++) {
		if (pObject3DList[i] != nullptr) {
			pObject3DList[i]->Update();
		}
	}
}

void Object3DManager::DeleteObject(Object3D* p)
{
	if (p == nullptr) {
		return;
	}

	auto itr = std::find(pObject3DList.begin(), pObject3DList.end(), p);
	if (itr != pObject3DList.end()) {
		pObject3DList.erase(itr);
	}
}

void Object3DManager::DeleteAllObject()
{
	pObject3DList.clear();
}

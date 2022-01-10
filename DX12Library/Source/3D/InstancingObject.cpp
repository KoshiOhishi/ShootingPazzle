#include "InstancingObject.h"
#include "BaseCollider.h"

using namespace DirectX;
using namespace Microsoft::WRL;

ComPtr<ID3D12RootSignature> InstancingObjectDraw::instancingFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::instancingFbxPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::shadowInstancingFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::shadowInstancingFbxPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::instancingObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::instancingObjPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::shadowInstancingObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::shadowInstancingObjPipelinestate = nullptr;

void InstancingObjectDraw::CreateGraphicsPipeline(int objectType, PipelineData& pipelineData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gsBlob;	//�W�I���g���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	assert(device);

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		pipelineData.vertexShaderFileName,    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
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
		result = D3DCompileFromFile(
			pipelineData.geometryShaderFileName,    // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "gs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&gsBlob, &errorBlob);
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
	result = D3DCompileFromFile(
		pipelineData.pixelShaderFileName,    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
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
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// �f�v�X�X�e���V���X�e�[�g
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;


	//���������̐[�x�l�������݋֎~
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2;    // �`��Ώۂ�2��
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

	if (objectType == ObjectType::OBJECTTYPE_INSTANCING_OBJ) {
		// ���[�g�V�O�l�`���̐���
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(instancingObjRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = instancingObjRootsignature.Get();

		// �O���t�B�b�N�X�p�C�v���C���̐���
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(instancingObjPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

	}
	else if (objectType == ObjectType::OBJECTTYPE_INSTANCING_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(instancingFbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = instancingFbxRootsignature.Get();

		// �O���t�B�b�N�X�p�C�v���C���̐���
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(instancingFbxPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }
	}

}

void InstancingObjectDraw::CreateShadowObjGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//�e�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Shader/InstancingOBJShadowTestVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);


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
	//�u�����h�X�e�[�g�̐ݒ�
	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(8 ���邪���܂͈�����g��Ȃ�)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // �W���ݒ�

	blenddesc.BlendEnable = true; // �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // �\�[�X�̒l�� 100% �g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // �f�X�g�̒l�� 0% �g��

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // ���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // �\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-�\�[�X�̃A���t�@�l

	//���������̐[�x�l�������݋֎~
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// �u�����h�X�e�[�g�ɐݒ肷��
	//gpipeline.BlendState.RenderTarget[0]


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
	//�萔�p
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0)

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
		IID_PPV_ARGS(shadowInstancingObjRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = shadowInstancingObjRootsignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowInstancingObjPipelinestate.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void InstancingObjectDraw::CreateShadowFbxGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//�e�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Shader/InstancingFBXShadowTestVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);


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
	//�u�����h�X�e�[�g�̐ݒ�
	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(8 ���邪���܂͈�����g��Ȃ�)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // �W���ݒ�

	blenddesc.BlendEnable = true; // �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // �\�[�X�̒l�� 100% �g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // �f�X�g�̒l�� 0% �g��

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // ���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // �\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-�\�[�X�̃A���t�@�l

	//���������̐[�x�l�������݋֎~
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// �u�����h�X�e�[�g�ɐݒ肷��
	//gpipeline.BlendState.RenderTarget[0]


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
	CD3DX12_ROOT_PARAMETER rootparams[4];
	//�萔�p
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0)
	//�e�N�X�`���p
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//�萔�p
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b1)
	//FBX�X�L�j���O
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //�萔�o�b�t�@�r���[�Ƃ��ď�����(b2)

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
		IID_PPV_ARGS(shadowInstancingFbxRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = shadowInstancingFbxRootsignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowInstancingFbxPipelinestate.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void InstancingObjectDraw::Initialize()
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

	for (int i = 0; i < _countof(constBuffTransforms); i++) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancing) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constBuffTransforms[i].ReleaseAndGetAddressOf())
		);
	}

	for (int i = 0; i < _countof(constBuffShadows); i++) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancingShadow) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constBuffShadows[i].ReleaseAndGetAddressOf())
		);
	}

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancingLightCamera) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffLightCamera.ReleaseAndGetAddressOf())
	);

	////�I�u�W�F�N�g�}�l�[�W���[�ɒǉ�
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//�N���X���̕�������擾
	name = typeid(*this).name();

	datas.resize(MAX_INSTANCECOUNT);
}

void InstancingObjectDraw::Update()
{
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

	if (objectType == OBJECTTYPE_INSTANCING_OBJ) {

		HRESULT result;

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
	}
	else if (objectType == OBJECTTYPE_INSTANCING_FBX) {

		//���f���̃��b�V���g�����X�t�H�[��
		XMMATRIX modelTransform = fbxModel->GetModelTransForm();

		HRESULT result;

		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->cameraPos = camera->GetPosition();;
			constBuffShare->Unmap(0, nullptr);
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

	//�����蔻��X�V
	if (collider) {
		collider->Update();
	}

	drawPhase = DRAW_PHASE_DRAWABLE;
}

void InstancingObjectDraw::DrawPrivate()
{
	if (drawCount == 0) {
		return;
	}

	//Update()��ʂ炸�ɕ`�悵�悤�Ƃ�����͂���
	if (drawPhase == DRAW_PHASE_UPDATEABLE) {
		//�J�E���g��0�ɖ߂�
		drawCount = 0;
		return;
	}

	//�萔�o�b�t�@�Ƀf�[�^�]��
	int lDrawCount = drawCount;
	int roopCount_i = lDrawCount / MAX_ONCEDRAWCOUNT + 1;
	InstancingObjectDraw::ConstBufferDataInstancing* constMap = nullptr;
	for (int i = 0; i < roopCount_i; i++) {

		HRESULT result = constBuffTransforms[i]->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {

			int roopCount_j = lDrawCount > 512 ? 512 : lDrawCount;
			for (int j = 0; j < roopCount_j; j++) {
				constMap->data[j].color = datas[i * MAX_ONCEDRAWCOUNT + j].color;
				constMap->data[j].world = datas[i * MAX_ONCEDRAWCOUNT + j].world;
			}

			if (objectType == OBJECTTYPE_INSTANCING_OBJ) {
				//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
				if (objModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//���[�g�V�O�l�`���̐ݒ�
					DX12Util::GetCmdList()->SetGraphicsRootSignature(instancingObjRootsignature.Get());
					//�p�C�v���C���X�e�[�g�̐ݒ�
					DX12Util::GetCmdList()->SetPipelineState(instancingObjPipelinestate.Get());
				}

				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransforms[i]->GetGPUVirtualAddress());
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
				objModel->Draw(roopCount_j);

				prevDrawObjectType = objectType;

			}
			else if (objectType == OBJECTTYPE_INSTANCING_FBX) {
				//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
				if (fbxModel == nullptr) {
					return;
				}
				if (objectType != prevDrawObjectType) {
					//���[�g�V�O�l�`���̐ݒ�
					DX12Util::GetCmdList()->SetGraphicsRootSignature(instancingFbxRootsignature.Get());
					//�p�C�v���C���X�e�[�g�̐ݒ�
					DX12Util::GetCmdList()->SetPipelineState(instancingFbxPipelinestate.Get());
				}
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransforms[i]->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
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
				fbxModel->Draw(roopCount_j);

				prevDrawObjectType = objectType;
			}

			lDrawCount -= roopCount_j;
			constBuffTransforms[i]->Unmap(0, nullptr);
		}
	}

	drawCount = 0;
	drawPhase = DRAW_PHASE_UPDATEABLE;
}

void InstancingObjectDraw::DrawShadow()
{
	if (drawCount == 0 || 
		isDrawShadowToOther == false) {
		return;
	}

	//Update()��ʂ炸�ɕ`�悵�悤�Ƃ�����͂���
	if (drawPhase == DRAW_PHASE_UPDATEABLE) {
		return;
	}

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

	//�萔�o�b�t�@�Ƀf�[�^���M
	InstancingObjectDraw::ConstBufferDataInstancingLightCamera* constMap = nullptr;
	HRESULT result = constBuffLightCamera->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->lightCamera = matLight;
		constBuffLightCamera->Unmap(0, nullptr);
	}

	//�萔�o�b�t�@�Ƀf�[�^�]��
	int lDrawCount = drawCount;
	int roopCount_i = lDrawCount / MAX_ONCEDRAWCOUNT + 1;
	InstancingObjectDraw::ConstBufferDataInstancingShadow* constMap2 = nullptr;
	for (int i = 0; i < roopCount_i; i++) {

		result = constBuffShadows[i]->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {

			int roopCount_j = lDrawCount > 512 ? 512 : lDrawCount;
			for (int j = 0; j < roopCount_j; j++) {
				constMap2->world[j] = datas[i * MAX_ONCEDRAWCOUNT + j].world;
			}

			if (objectType == OBJECTTYPE_INSTANCING_OBJ) {
				//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
				if (objModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//���[�g�V�O�l�`���̐ݒ�
					DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowInstancingObjRootsignature.Get());

					//�p�C�v���C���X�e�[�g�̐ݒ�
					DX12Util::GetCmdList()->SetPipelineState(shadowInstancingObjPipelinestate.Get());
				}
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadows[i]->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffLightCamera->GetGPUVirtualAddress());

				//���f���`��
				objModel->Draw(roopCount_j, true);

			}
			else if (objectType == OBJECTTYPE_INSTANCING_FBX) {
				//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
				if (fbxModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//���[�g�V�O�l�`���̐ݒ�
					DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowInstancingFbxRootsignature.Get());

					//�p�C�v���C���X�e�[�g�̐ݒ�
					DX12Util::GetCmdList()->SetPipelineState(shadowInstancingFbxPipelinestate.Get());
				}
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadows[i]->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffLightCamera->GetGPUVirtualAddress());
				//�萔�o�b�t�@�r���[���Z�b�g
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(3, constBuffSkin->GetGPUVirtualAddress());
				//���f���`��
				fbxModel->Draw(roopCount_j, true);
			}

			constBuffShadows[i]->Unmap(0, nullptr);
		}
	}
}

void InstancingObjectDraw::SetFbxModel(FbxModel* fbxModel)
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

	objectType = OBJECTTYPE_INSTANCING_FBX;
}

void InstancingObjectDraw::SetObjModel(ObjModel* objModel)
{
	this->objModel = objModel;

	objectType = OBJECTTYPE_INSTANCING_OBJ;
}

void InstancingObjectDraw::AddInstancingData(const InstanceData& constBuffData)
{
	//DrawPrivate()��ʉ߂��Ȃ��Ńf�[�^�ǉ����悤�Ƃ����Ƃ��A
	//���̃t���[���ł̍ŏ��̌Ăяo���ɂȂ�̂�
	//�R���e�i�����Ȃ��悤��drawCount��0�ɖ߂��Ă�����
	if (drawPhase == DRAW_PHASE_DRAWABLE) {
		drawCount = 0;
		drawPhase = DRAW_PHASE_UPDATEABLE;
	}

	datas[drawCount] = constBuffData;
	drawCount++;
}

void InstancingObject::Initialize(const Vector3& pos, const Vector3& rot, const Vector3& scale, const Vector4& color)
{
	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);
	SetColor(color);
}

void InstancingObject::Update(InstancingObjectDraw& instancingObjectDraw)
{
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

	instancingObjectDraw.AddInstancingData({ color, matWorld });
}
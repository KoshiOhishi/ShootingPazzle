#include "Particle2D.h"
#include "DX12Util.h"
#include "DrawManager.h"
#include "Archive.h"
#include "Encorder.h"

using namespace DirectX;
using namespace DX12Library;

Microsoft::WRL::ComPtr<ID3D12RootSignature> Particle2D::rootsignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Particle2D::pipelinestate[PIPELINE_COUNT];
DirectX::XMMATRIX Particle2D::matProjection;

void Particle2D::StaticInitialize()
{
	// �p�C�v���C��������
	CreateGraphicsPipeline();

	//�s�񏉊���
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f
	);
}

void Particle2D::BeginDraw()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelinestate[0].Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void Particle2D::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gsBlob;	// �W�I���g���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	ID3D12Device* device = DX12Util::GetDevice();

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/Particle2DVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/Particle2DVS.hlsl"));

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
			L"Shader/Particle2DVS.hlsl",    // �V�F�[�_�t�@�C����
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
	bool isLoadedArchiveGS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/Particle2DGS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/Particle2DGS.hlsl"));

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
			L"Shader/Particle2DGS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "gs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
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

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	bool isLoadedArchivePS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/Particle2DPS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/Particle2DPS.hlsl"));

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
			L"Shader/Particle2DPS.hlsl",    // �V�F�[�_�t�@�C����
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

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//�X�P�[��
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//�f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//��U�W���l���Z�b�g
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//��ɏ㏑�����[��
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;

	//����������
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	//���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//���������̐[�x�l�������ݐݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = rootsignature.Get();

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
		case PARTICLE_BLENDMODE_NORMAL:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case PARTICLE_BLENDMODE_ADD:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case PARTICLE_BLENDMODE_SUB:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		case PARTICLE_BLENDMODE_MUL:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case PARTICLE_BLENDMODE_SCREEN:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case PARTICLE_BLENDMODE_REVERSE:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}

		//���������̐[�x�l�������ݐݒ�
		gpipeline.BlendState.AlphaToCoverageEnable = true;

		// �u�����h�X�e�[�g�ɐݒ肷��
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		//�p�C�v���C���X�e�[�g�̐���
		result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate[i].ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	}
}

void Particle2D::CreateModel()
{
	HRESULT result = S_FALSE;
	ID3D12Device* device = DX12Util::GetDevice();

	// ���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	if (FAILED(result)) {
		assert(0);
		return;
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPos2D* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);
}

void Particle2D::Initialize()
{
	HRESULT result;
	ID3D12Device* device = DX12Util::GetDevice();

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData2D) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	// ���f������
	CreateModel();
}

void Particle2D::Update()
{
	HRESULT result;

	//�������s�����p�[�e�B�N����S�폜
	particles.remove_if(
		[](Particle& x) {
			return x.life.GetIsEnd();
		}
	);

	//�S�p�[�e�B�N���X�V
	for (std::forward_list<Particle>::iterator it = particles.begin();
		it != particles.end(); it++)
	{
		////���ԍX�V
		//it->life.Update();
		//���x�ɉ����x�����Z
		it->velocity = it->velocity + it->accel;
		//���x�ɂ��ړ�
		it->position = it->position + it->velocity;

		//�X�P�[���X�V
		//�i�s�x��0�`1�͈̔͂Ɋ��Z
		float f = (float)it->life.GetNowTime() / it->life.GetEndTime();

		//�X�P�[��
		if (f < 0.5f) {
			it->scale = it->s_scale + (it->m_scale - it->s_scale) * f * 2.0f;
		}
		else {
			it->scale = it->m_scale + (it->e_scale - it->m_scale) * (f - 0.5f) * 2.0f;
		}
	}

	D3D12_RESOURCE_DESC resDesc =
		texbuff[texNumber]->GetDesc();

	//���_�o�b�t�@�փf�[�^�]��
	VertexPos2D* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		//�p�[�e�B�N���̏���1�����f
		for (std::forward_list<Particle>::iterator it = particles.begin();
			it != particles.end(); it++)
		{
			//���W
			vertMap->pos = { it->position.x, it->position.y, 0 };
			//�X�P�[��
			vertMap->scale = Vector2(resDesc.Width, resDesc.Height) * it->scale;
			//���̒��_��
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData2D* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->mat = matProjection;	//�v���W�F�N�V�����s��]��
		constMap->color = color;
		constBuff->Unmap(0, nullptr);
	}
}

void Particle2D::Draw()
{
	DrawManager::AddDrawList(DRAW_MANAGER_OBJECT_TYPE_PARTICLE2D, this);
}

void Particle2D::DrawPrivate()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();

	//�O��̕`��Ɏg�p���ꂽ�p�C�v���C���̃C���f�b�N�X�ƍ���g�����̂��Ⴄ�Ȃ�
	//�����Ő؂�ւ�
	static int prevPipelineIndex = 0;
	if (pipelineIndex != prevPipelineIndex) {
		//�p�C�v���C���X�e�[�g�̐ݒ�
		DX12Util::GetCmdList()->SetPipelineState(pipelinestate[pipelineIndex].Get());
	}

	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	//cmdList->IASetIndexBuffer(&ibView);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1, 
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			descriptorHandleIncrementSize)
	);
	// �`��R�}���h
	//cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
	cmdList->DrawInstanced((UINT)std::distance(particles.begin(), particles.end()), 1, 0, 0);
}

void Particle2D::Add(double life, const Vector2& position, const Vector2& velocity, const Vector2& accel, float start_scale, float end_scale)
{
	//���X�g�ɗv�f��ǉ�
	particles.emplace_front();
	//�ǉ������v�f�̎Q��
	Particle& p = particles.front();
	//�l�̃Z�b�g
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.scale = start_scale;
	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.m_scale = start_scale + (end_scale - start_scale) * 0.5f;
	p.life.SetTimer(0, life);
	p.life.Start();
}

void Particle2D::Add(double life, const Vector2& position, const Vector2& velocity, const Vector2& accel, float start_scale, float end_scale, float middle_scale)
{
	//���X�g�ɗv�f��ǉ�
	particles.emplace_front();
	//�ǉ������v�f�̎Q��
	Particle& p = particles.front();
	//�l�̃Z�b�g
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.scale = start_scale;
	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.m_scale = middle_scale;
	p.life.SetTimer(0, life);
	p.life.Start();
}
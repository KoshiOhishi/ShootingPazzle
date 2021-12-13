#include "Object3D.h"
#include "DX12Util.h"
#include "BaseCollider.h"
#include "CollisionManager.h"

#include <typeinfo>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Object3D::device = nullptr;
Camera* Object3D::camera = nullptr;
Light* Object3D::light = nullptr;
ComPtr <ID3D12DescriptorHeap> Object3D::basicDescHeap = nullptr;
ComPtr<ID3D12RootSignature> Object3D::fbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::fbxPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> Object3D::objRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::objPipelinestate = nullptr;
std::vector<Object3D*> Object3DManager::pObject3DList;


void Object3D::CreateDescHeap()
{
	HRESULT result;

	//�f�X�N���v�^�q�[�v�𐶐�
	// �ݒ�\����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // �V�F�[�_�[���猩����
	descHeapDesc.NumDescriptors = 32; // SRV1��
	// ����
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(basicDescHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
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
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

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

	//���������̐[�x�l�������݋֎~
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�2��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA

	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(pipelineData.rootparams.size(), pipelineData.rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	if (objectType == ObjectType::OBJECTTYPE_OBJ) {
		// ���[�g�V�O�l�`���̐���
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(objRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = objRootsignature.Get();

		// �O���t�B�b�N�X�p�C�v���C���̐���
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(objPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

	}
	else if (objectType == ObjectType::OBJECTTYPE_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(fbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = fbxRootsignature.Get();

		// �O���t�B�b�N�X�p�C�v���C���̐���
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(fbxPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }
	}


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
	
	////�I�u�W�F�N�g�}�l�[�W���[�ɒǉ�
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//�N���X���̕�������擾
	name = typeid(*this).name();
}

void Object3D::Update()
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

	if (objectType == OBJECTTYPE_OBJ) {

		//�r���[�v���W�F�N�V�����s��
		const XMMATRIX& matViewProjection = camera->GetViewProjection();
		//�J�������W
		const Vector3& cameraPos = camera->GetPosition();

		HRESULT result;

		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->cameraPos = cameraPos;
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

		//�r���[�v���W�F�N�V�����s��
		const XMMATRIX& matViewProjection = camera->GetViewProjection();
		//���f���̃��b�V���g�����X�t�H�[��
		const XMMATRIX& modelTransform = fbxModel->GetModelTransForm();
		//�J�������W
		const Vector3& cameraPos = camera->GetPosition();

		HRESULT result;

		//�萔�o�b�t�@�փf�[�^�]��
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->cameraPos = cameraPos;
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

	//�����蔻��X�V
	if (collider) {
		collider->Update();
	}
}

void Object3D::Draw()
{
	if (objectType == OBJECTTYPE_OBJ) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (objModel == nullptr) {
			return;
		}

		//�p�C�v���C���X�e�[�g�̐ݒ�
		DX12Util::GetCmdList()->SetPipelineState(objPipelinestate.Get());
		//���[�g�V�O�l�`���̐ݒ�
		DX12Util::GetCmdList()->SetGraphicsRootSignature(objRootsignature.Get());

		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//���C�g�̕`��
		light->Draw(3);
		//���f���`��
		objModel->Draw();

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
		if (fbxModel == nullptr) {
			return;
		}

		//�p�C�v���C���X�e�[�g�̐ݒ�
		DX12Util::GetCmdList()->SetPipelineState(fbxPipelinestate.Get());
		//���[�g�V�O�l�`���̐ݒ�
		DX12Util::GetCmdList()->SetGraphicsRootSignature(fbxRootsignature.Get());
		//�v���~�e�B�u�`���ݒ�
		DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
		//���C�g�̕`��
		light->Draw(3);
		//���f���`��
		fbxModel->Draw();
	}


}

void Object3D::PlayAnimation()
{
	if (objectType == OBJECTTYPE_OBJ) {
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

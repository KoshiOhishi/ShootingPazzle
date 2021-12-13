#include "RenderText.h"
#include "DX12Util.h"


//���[�g�V�O�l�`��
Microsoft::WRL::ComPtr<ID3D12RootSignature> RenderText::rootSignature;
//�p�C�v���C���X�e�[�g
Microsoft::WRL::ComPtr<ID3D12PipelineState> RenderText::pipelineState;
//�ˉe�s��
DirectX::XMMATRIX RenderText::matProjection;
//���_�V�F�[�_�I�u�W�F�N�g
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::vsBlob;
//�s�N�Z���V�F�[�_�I�u�W�F�N�g
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::psBlob;
//�G���[�I�u�W�F�N�g
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::errorBlob;
//�f�X�N���v�^�q�[�v
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RenderText::descHeap;
//������
int RenderText::createdCount = 0;

void RenderText::StaticInitialize()
{
	HRESULT result = S_FALSE;

	//�e�V�F�[�_�t�@�C���̓ǂݍ��݂ƃR���p�C��
	vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	errorBlob = nullptr; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Shader/TextVertexShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"VSmain", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
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
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Shader/TextPixelShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"PSmain", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);


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
		exit(1);
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

		{"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},

	};

	//�O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\���̂�p�ӂ���B
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());


	//�T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	//���X�^���C�Y�X�e�[�g
	//�W���I�Ȑݒ�(�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//��U�W���l���Z�b�g
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//�J�����O���Ȃ�

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//��U�W���l���Z�b�g
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//��ɏ㏑�����[��
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

	// �u�����h�X�e�[�g�ɐݒ肷��
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//���������̐[�x�l�������݋֎~
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	//���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`����O�p�`�ɐݒ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	gpipeline.NumRenderTargets = 1; // �`��Ώۂ� 1 ��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255 �w��� RGBA
	gpipeline.SampleDesc.Count = 1; // 1 �s�N�Z���ɂ� 1 ��T���v�����O


	//�f�X�N���v�^�e�[�u���̐ݒ�
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 ���W�X�^

	//���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[2];
	//�萔�p
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0)
	//�e�N�X�`���p
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//�T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT);

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
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = rootSignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 512;
	result = DX12Util::GetDevice()->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf())
	);
}

void RenderText::SetFontData(FontData fontData)
{
	fd = fontData;
	changeFD = true;
}

void RenderText::FirstInit(wstring str)
{
	HRESULT result = S_FALSE;

	//���_�o�b�t�@����
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);

	prevStr = str;
}

void RenderText::CreateFontTexture(FontData fontData, wstring str)
{
	HRESULT result = S_FALSE;

	if (number == -1) {
		number = createdCount;
		createdCount++;
	}

	//�܂��̓t�H���g�쐬
	LOGFONT lf = {
		fontData.height, fontData.width, 0, 0,
		fontData.weight, fontData.italic, 0, 0,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		fontData.quality, DEFAULT_PITCH,
		L""
	};
	
	//�t�H���g���i�[
	for (int i = 0; i < 32; i++) {
		lf.lfFaceName[i] = fontData.fontName.data()[i];
	}

	HFONT hFont = CreateFontIndirectW(&lf);

	// ���݂̃E�B���h�E�ɓK�p
	// �f�o�C�X�Ƀt�H���g���������Ȃ���GetGlyphOutline�֐��̓G���[�ƂȂ�
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	const int gradFlag = GGO_GRAY4_BITMAP;

	//�K���̍ő�l
	int grad = 0;
	switch (gradFlag) {

	case GGO_GRAY2_BITMAP:
		grad = 4;
		break;

	case GGO_GRAY4_BITMAP:
		grad = 16;
		break;

	case GGO_GRAY8_BITMAP:
		grad = 64;
		break;
	}

	//�t�H���g�̃r�b�g�}�b�v�擾
	struct BitmapData {
		BYTE* pData;
		TEXTMETRIC tm;
		GLYPHMETRICS gm;
		DWORD size;
	};

	std::vector<BitmapData> bitmapDatas(str.size());
	//��������e�N�X�`���T�C�Y
	int texWidth = 0, texHeight = 0;

	for (int i = 0; i < str.size(); i++) {

		UINT code = (UINT)str.c_str()[i];

		TEXTMETRIC tm;
		GetTextMetricsW(hdc, &tm);
		GLYPHMETRICS gm;
		const MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };

		//�T�C�Y�擾
		DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &gm, 0, NULL, &mat);

		//�r�b�g�}�b�v�擾
		BYTE* pData = new BYTE[size];
		GetGlyphOutlineW(hdc, code, gradFlag, &gm, size, pData, &mat);

		//���X�g�ɒǉ�
		bitmapDatas[i].pData = pData;
		bitmapDatas[i].gm = gm;
		bitmapDatas[i].tm = tm;
		bitmapDatas[i].size = size;

		//�t�H���g�̕��ƍ��������Ƀe�N�X�`���T�C�Y���X�V
		int fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = tm.tmAscent + tm.tmDescent;
		
		if (fontWidth > gm.gmCellIncX) {
			texWidth += fontWidth;
		}
		else {
			texWidth += gm.gmCellIncX;
		}

		if (texHeight < fontHeight) {
			texHeight = fontHeight;
		}
	}

	//�f�o�C�X�R���e�L�X�g�A�t�H���g�n���h�����
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//�t�H���g���������ނ��߂̃e�N�X�`���쐬
	//GPU���Ƀe�N�X�`���o�b�t�@���쐬����
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		texWidth,
		(UINT)texHeight,
		1,
		1
	);

	//�e�N�X�`���o�b�t�@����
	result = DX12Util::GetDevice()->CreateCommittedResource(	//GPU���\�[�X�̐���
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(texbuff.ReleaseAndGetAddressOf())
	);

	//�e�N�X�`���쐬
	std::vector<TextureData> textureDatas(texWidth * texHeight);
	int offsetX = 0;

	for (int i = 0; i < bitmapDatas.size(); i++) {

		//�󔒂̂Ƃ��̓I�t�Z�b�g�������Z���Ď���
		if (bitmapDatas[i].gm.gmBlackBoxX <= 1 && bitmapDatas[i].gm.gmBlackBoxY <= 1) {
			offsetX += bitmapDatas[i].gm.gmCellIncX;
			continue;
		}

		//�t�H���g�̕��ƍ���
		int fontWidth = (bitmapDatas[i].gm.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = bitmapDatas[i].gm.gmBlackBoxY;
		//�����I�t�Z�b�g
		int offsetY = bitmapDatas[i].tm.tmAscent - bitmapDatas[i].gm.gmptGlyphOrigin.y;

		// �t�H���g���̏�������
		DWORD Alpha;
		for (int y = 0; y < fontHeight; y++)
		{
			for (int x = 0; x < fontWidth; x++)
			{
				Alpha = (255 * bitmapDatas[i].pData[y * fontWidth + x]) / grad;
				int index = (y + offsetY) * texWidth + offsetX + x + bitmapDatas[i].gm.gmptGlyphOrigin.x;

				if (textureDatas[index].A == 0) {
					textureDatas[index].A = Alpha * color.w;
				}
			}
		}

		offsetX += bitmapDatas[i].gm.gmCellIncX;
	}

	////�e�X�g �l���̓_���������ăe�N�X�`���T�C�Y�𖾎��I�ɂ���
	//textureDatas[0].R = textureDatas[0].G = textureDatas[0].B = 0;
	//textureDatas[texWidth - 1].R = textureDatas[texWidth - 1].G = textureDatas[texWidth - 1].B = 0;
	//textureDatas[texWidth * (texHeight - 1)].R = textureDatas[texWidth * (texHeight - 1)].G = textureDatas[texWidth * (texHeight - 1)].B = 0;
	//textureDatas[texWidth * texHeight - 1].R = textureDatas[texWidth * texHeight - 1].G = textureDatas[texWidth * texHeight - 1].B = 0;

	//textureDatas[0].A = 255;
	//textureDatas[texWidth - 1].A = 255;
	//textureDatas[texWidth * (texHeight - 1)].A = 255;
	//textureDatas[texWidth * texHeight - 1].A = 255;

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texbuff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�ɃR�s�[
		textureDatas.data(),	//���f�[�^�A�h���X
		sizeof(TextureData) * texWidth,	//1���C���T�C�Y
		sizeof(TextureData) * textureDatas.size()	//�S�T�C�Y
	);

	//�r�b�g�}�b�v�f�[�^���
	for (int i = 0; i < bitmapDatas.size(); i++) {
		delete[] bitmapDatas[i].pData;
	}
	bitmapDatas.clear();
	textureDatas.clear();

	//���ۂɃV�F�[�_�[���\�[�X�r���[�𐶐�����
	//�V�F�[�_�[���\�[�X�r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
	srvDesc.Format = texresDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	DX12Util::GetDevice()->CreateShaderResourceView(
		texbuff.Get(),	//�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,	//�e�N�X�`���ݒ���
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			number,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);


	//���_�f�[�^
	VertexPosUv vertices[] = {
		//	 x		y	  z		u	  v
		{{	0.0f,			(float)texHeight,	0.0f},	{0.0f,1.0f}},	//����
		{{	0.0f,			0.0f,				0.0f},	{0.0f,0.0f}},	//����
		{{(float)texWidth,	(float)texHeight,	0.0f},	{1.0f,1.0f}},	//�E��
		{{(float)texWidth,  0.0f,				0.0f},	{1.0f,0.0f}},	//�E��

	};


	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * vertNum;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	//�萔�o�b�t�@�̐���
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);

	//�萔�o�b�t�@�Ƀf�[�^��]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;		//�F�w�� (RGBA)
	constMap->mat = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f);	//���s���e�s��̍���
	constBuff->Unmap(0, nullptr);

	//�s�񏉊���
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f
	);

	changeFD = false;
}

bool RenderText::IsReCreate(wstring str)
{
	bool flag = false;

	if (str != prevStr) {
		flag = true;
	}

	prevStr = str;

	return flag || changeFD;
}

void RenderText::DrawString(float x, float y, wstring str)
{
	//��������Ă��Ȃ������炱���Ő���
	if (number == -1) {
		FirstInit(str);
		CreateFontTexture(fd, str);
	}
	//�����ύX�ȂǂōĐ������K�v�Ȃ�Đ���
	else if (IsReCreate(str)) {
		CreateFontTexture(fd, str);
	}

	//�p�C�v���C���X�e�[�g�̐ݒ�
	DX12Util::GetCmdList()->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	DX12Util::GetCmdList()->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//���W���
	position = DirectX::XMVectorSet(x, y, 0, 0);

	//���[���h�s��̍X�V
	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	matWorld *= DirectX::XMMatrixTranslationFromVector(position);

	//�s��̓]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = matWorld * matProjection;		//�s��̍���
	constBuff->Unmap(0, nullptr);


	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//���_�o�b�t�@���Z�b�g
	DX12Util::GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());


	//�V�F�[�_���\�[�X�r���[���Z�b�g
	DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			number,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//�`��R�}���h
	DX12Util::GetCmdList()->DrawInstanced(4, 1, 0, 0);

}

void RenderText::SetColor(float r, float g, float b, float a)
{
	color.x = r;
	color.y = g;
	color.z = b;
	color.w = a;
}

void RenderText::SetColorAs0To255(float r, float g, float b, float a)
{
	SetColor(r / DX12TEXT_COLOR_MAX_RATE, g / DX12TEXT_COLOR_MAX_RATE, b / DX12TEXT_COLOR_MAX_RATE, a / DX12TEXT_COLOR_MAX_RATE);
}

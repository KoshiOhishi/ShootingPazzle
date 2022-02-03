#include <Windows.h>
#include <wrl.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <shlwapi.h>
#include "DX12Util.h"
#include "ImguiHelper.h"
#include "Input.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Sound.h"
#include "DX12Util.h"
#include "SceneManager.h"
#include "FPSManager.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "Timer.h"
#include "ImguiHelper.h"
#include "Object3D.h"
#include "InstancingObject.h"
#include "RenderText.h"
#include "ParticleManager.h"
#include "Particle3D.h"
#include "Particle2D.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "shlwapi.lib")

int DX12Util::window_width = 1280;// ����
int DX12Util::window_height = 720; // �c��

WNDCLASSEX DX12Util::w; // �E�B���h�E�N���X�̐ݒ�
HWND DX12Util::hwnd;
LPARAM DX12Util::lparam;
DX12Util::ComPtr<ID3D12Device> DX12Util::dev;
DX12Util::ComPtr<IDXGIFactory6> DX12Util::dxgiFactory;
DX12Util::ComPtr<IDXGISwapChain4> DX12Util::swapchain;
DX12Util::ComPtr<ID3D12CommandAllocator> DX12Util::cmdAllocator;
DX12Util::ComPtr<ID3D12GraphicsCommandList> DX12Util::cmdList;
DX12Util::ComPtr<ID3D12CommandQueue> DX12Util::cmdQueue;
DX12Util::ComPtr<ID3D12DescriptorHeap> DX12Util::rtvHeaps;
D3D12_DESCRIPTOR_HEAP_DESC DX12Util::heapDesc;
std::vector<DX12Util::ComPtr<ID3D12Resource>> DX12Util::backBuffers;
DX12Util::ComPtr<ID3D12Resource> DX12Util::depthBuffer[2];
DX12Util::ComPtr <ID3D12DescriptorHeap> DX12Util::dsvHeap;
DX12Util::ComPtr<ID3D12Fence> DX12Util::fence;
UINT64 DX12Util::fenceVal;

//Imgui
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY: //�E�B���h�E���j�����ꂽ
		PostQuitMessage(0); //OS�ɑ΂��āA�A�v���̏I����`����
		return 0;

	case WM_PAINT:

		return 0;
	}

	DX12Util::SetLParam(lparam);

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	return DefWindowProc(hwnd, msg, wparam, lparam); //�W���̏������s��
}

void DX12Util::Initialize(const wchar_t* windowName, int windowWidth, int windowHeight)
{
	window_width = windowWidth;
	window_height = windowHeight;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // �E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = windowName; // �E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr); // �E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // �J�[�\���w��
	// �E�B���h�E�N���X�� OS �ɓo�^
	RegisterClassEx(&w);
	// �E�B���h�E�T�C�Y{ X ���W Y ���W ���� �c�� }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �����ŃT�C�Y�␳
	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(w.lpszClassName, // �N���X��
		windowName, // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW, // �W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT, // �\�� X ���W(OS �ɔC����)
		CW_USEDEFAULT, // �\�� Y ���W(OS �ɔC����)
		wrc.right - wrc.left, // �E�B���h�E����
		wrc.bottom - wrc.top, // �E�B���h�E�c��
		nullptr, // �e�E�B���h�E�n���h��
		nullptr, // ���j���[�n���h��
		w.hInstance, // �Ăяo���A�v���P�[�V�����n���h��
		nullptr); // �I�v�V����
		// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	// DirectX ���������� ��������
	HRESULT result;
	dev = nullptr;

	// DXGI �t�@�N�g���[�̐���
#ifdef _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
#endif
	// �A�_�v�^�[�̗񋓗p
	std::vector< ComPtr<IDXGIAdapter>> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr < IDXGIAdapter> tmpAdapter = nullptr;
	for (int i = 0;
		dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter); // ���I�z��ɒǉ�����
	}
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		adapters[i]->GetDesc(&adesc); // �A�_�v�^�[�̏����擾
		std::wstring strDesc = adesc.Description; // �A�_�v�^�[��
		// Microsoft Basic Render Driver �����
		if (strDesc.find(L"Microsoft") == std::wstring::npos && strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; // �̗p
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(dev.ReleaseAndGetAddressOf()));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

	// �R�}���h�A���P�[�^�𐶐�
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdAllocator.ReleaseAndGetAddressOf()));
	// �R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf()));

	// �W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf()));

	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
	swapchainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2; // �o�b�t�@����2�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1 ��ComPtr��p��
	ComPtr<IDXGISwapChain1> swapchain1;
	//�X���b�v�`�F�[���̐���
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);
	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapchain1.As(&swapchain);

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);

	// �e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2; // ���\��2��
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps.ReleaseAndGetAddressOf()));
	backBuffers.resize(swcDesc.BufferCount);
	// ���\��2���ɂ���
	for (int i = 0; i < 2; i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffers[i].ReleaseAndGetAddressOf()));
		// �f�B�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i,
				dev.Get()->GetDescriptorHandleIncrementSize(heapDesc.Type));
		// �����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);

	}

	// �t�F���X�̐���
	fenceVal = 0;
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));

	//�[�x�o�b�t�@�̍쐬
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32_TYPELESS,
		DX12Util::GetWindowWidth(),
		DX12Util::GetWindowHeight(),
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//���\�[�X����
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer[0])
	);
	//���\�[�X����
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer[1])
	);


	//�R�}���h���s�̂��߂ɐ[�x�r���[�𐶐�����
	//�[�x�r���[�p�f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapdesc{};
	dsvHeapdesc.NumDescriptors = 2;	//�[�x�r���[��1��
	dsvHeapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	result = DX12Util::GetDevice()->CreateDescriptorHeap(
		&dsvHeapdesc,
		IID_PPV_ARGS(dsvHeap.ReleaseAndGetAddressOf())
	);

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DX12Util::GetDevice()->CreateDepthStencilView(
		depthBuffer[0].Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//�[�x�r���[�쐬
	DX12Util::GetDevice()->CreateDepthStencilView(
		depthBuffer[1].Get(),
		&dsvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			dsvHeap->GetCPUDescriptorHandleForHeapStart(), 1, DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV))
	);


	//ImgUi������
	ImguiHelper::Initialize();

	//FbxLoader������
	FbxLoader::GetInstance()->Initialize(DX12Util::GetDevice());

	//�C���v�b�g������
	Input::Initialize(DX12Util::GetHwnd());

	//�T�E���h(XAudio2������)
	Sound::StaticInitialize(true);

	//�f�o�C�X�Z�b�g
	Object3D::SetDevice(DX12Util::GetDevice());

	//�q�[�v����
	Object3D::StaticInitialize();

	//�X�v���C�g������
	Sprite::FirstInit();

	//�����`��N���X������
	RenderText::StaticInitialize();

	//�f�o�b�O�e�L�X�g������
	DebugText::Initialize(L"Resources/System/debugfont.png");

	//�p�[�e�B�N��������
	ParticleManager::StaticInitialize();
	Particle3D::StaticInitialize();
	Particle2D::StaticInitialize();

}

void DX12Util::PreDrawBB()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾(2 �Ȃ̂� 0 �Ԃ� 1 ��)
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	// 1.���\�[�X�o���A��ύX
	//�\����Ԃ���`���ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 2-1.�����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	
	//2-2.�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//�����_�[�^�[�Q�b�g�Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// 3.��ʃN���A�R�}���h��������

	// �S��ʃN���A R G B A		
	float clearColor[4] = { 0.1,0.25,0.5};
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	// 3.��ʃN���A�R�}���h�����܂�

	//4.�r���[�|�[�g�̐ݒ�R�}���h
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, window_width, window_height));
	//5.�V�U�[��`�̐ݒ�R�}���h
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, window_width, window_height));
}

void DX12Util::PostDrawBB()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾(2 �Ȃ̂� 0 �Ԃ� 1 ��)
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//�`���Ԃ���\����ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//11.���߂̃N���[�Y
	cmdList->Close();

	//12.�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; // �R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	//13.�o�b�t�@���t���b�v(���\�̓��ւ�)
	swapchain->Present(1, 0);

	//14.�R�}���h���X�g�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//15.�R�}���h�A���P�[�^�[�̃��Z�b�g
	cmdAllocator->Reset(); // �L���[���N���A

	//16.�R�}���h���X�g�̃��Z�b�g
	cmdList->Reset(cmdAllocator.Get(), nullptr); // �ĂуR�}���h���X�g�𒙂߂鏀��
}

void DX12Util::End()
{
	Sound::StaticFinalize();
	SceneManager::DeleteScene();
	FbxLoader::GetInstance()->Finalize();
	ImguiHelper::Finalize();

	//�R�}���h���X�g�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	for (UINT i = 0; i < backBuffers.size(); i++){
		backBuffers[i].Reset();
	}

	depthBuffer[0].Reset();
	depthBuffer[1].Reset();

	swapchain.Reset();
	fence.Reset();
	cmdList.Reset();
	cmdAllocator.Reset();
	cmdQueue.Reset();
	dev.Reset();

	UnregisterClass(w.lpszClassName, w.hInstance);
}

HWND DX12Util::GetHwnd()
{
	return hwnd;
}

ID3D12Device* DX12Util::GetDevice()
{
	return dev.Get();
}


ID3D12GraphicsCommandList* DX12Util::GetCmdList()
{
	return cmdList.Get();
}

LPARAM& DX12Util::GetLParam()
{
	return lparam;
}

void DX12Util::SetLParam(const LPARAM& lparam)
{
	DX12Util::lparam = lparam;
}

const int DX12Util::GetWindowWidth()
{
	return window_width;
}

const int DX12Util::GetWindowHeight()
{
	return window_height;
}

bool DX12Util::IsFileExist(const wchar_t * path)
{
	return PathFileExists(path);
}

void DX12Util::ClearDepthBuffer(bool isShadow)
{
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart(), isShadow, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
	// �[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}


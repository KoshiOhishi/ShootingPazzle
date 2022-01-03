#pragma once
#include <Windows.h>
#include <vector>
#include <wrl.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include "DX12Util.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class DX12Util
{
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMVECTOR = DirectX::XMVECTOR;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //�T�u�N���X
	struct PipelineData
	{
		//���_�V�F�[�_�t�@�C����
		const wchar_t* vertexShaderFileName = nullptr;
		//�W�I���g���V�F�[�_�t�@�C����
		const wchar_t* geometryShaderFileName = nullptr;
		//�s�N�Z���V�F�[�_�t�@�C����
		const wchar_t* pixelShaderFileName = nullptr;
		//���_���C�A�E�g
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		// �f�X�N���v�^�����W
		std::vector<CD3DX12_DESCRIPTOR_RANGE> descRanges;
		//���[�g�p�����[�^
		std::vector<CD3DX12_ROOT_PARAMETER> rootparams;
	};

private:
	// �E�B���h�E�T�C�Y
	static int window_width;
	static int window_height;

	static WNDCLASSEX w; // �E�B���h�E�N���X�̐ݒ�
	static HWND hwnd;
	static LPARAM lparam;
	static ComPtr<ID3D12Device> dev;
	static ComPtr<IDXGIFactory6> dxgiFactory;
	static ComPtr<IDXGISwapChain4> swapchain;
	static ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static ComPtr<ID3D12GraphicsCommandList> cmdList;
	static ComPtr<ID3D12CommandQueue> cmdQueue;
	static ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static std::vector<ComPtr<ID3D12Resource>> backBuffers;
	static ComPtr<ID3D12Resource> depthBuffer;
	static ComPtr <ID3D12DescriptorHeap> dsvHeap;
	static ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;

public:
	static void Initialize(const wchar_t* windowName = L"DirectXGame", int windowWidth = 1280, int windowHeight = 720);
	static void BeginDraw();
	static void EndDraw();
	static void End();

	static HWND GetHwnd();
	static ID3D12Device* GetDevice();
	static ID3D12GraphicsCommandList* GetCmdList();
	static LPARAM& GetLParam();
	static void SetLParam(const LPARAM& lparam);

	static const int GetWindowWidth();
	static const int GetWindowHeight();

	static ID3D12Resource* GetDepthBuffer() { return depthBuffer.Get(); }
	static ID3D12DescriptorHeap* GetDepthHeap() { return dsvHeap.Get(); }
	/// <summary>
	/// �t�@�C�������݂��邩��Ԃ�
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns>�t�@�C�������݂��邩�ۂ�</returns>
	static bool IsFileExist(const wchar_t* path);

	static void ClearDepthBuffer();

};


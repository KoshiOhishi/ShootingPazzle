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

int DX12Util::window_width = 1280;// 横幅
int DX12Util::window_height = 720; // 縦幅

WNDCLASSEX DX12Util::w; // ウィンドウクラスの設定
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
	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY: //ウィンドウが破棄された
		PostQuitMessage(0); //OSに対して、アプリの終了を伝える
		return 0;

	case WM_PAINT:

		return 0;
	}

	DX12Util::SetLParam(lparam);

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	return DefWindowProc(hwnd, msg, wparam, lparam); //標準の処理を行う
}

void DX12Util::Initialize(const wchar_t* windowName, int windowWidth, int windowHeight)
{
	window_width = windowWidth;
	window_height = windowHeight;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定
	w.lpszClassName = windowName; // ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // カーソル指定
	// ウィンドウクラスを OS に登録
	RegisterClassEx(&w);
	// ウィンドウサイズ{ X 座標 Y 座標 横幅 縦幅 }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // 自動でサイズ補正
	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(w.lpszClassName, // クラス名
		windowName, // タイトルバーの文字
		WS_OVERLAPPEDWINDOW, // 標準的なウィンドウスタイル
		CW_USEDEFAULT, // 表示 X 座標(OS に任せる)
		CW_USEDEFAULT, // 表示 Y 座標(OS に任せる)
		wrc.right - wrc.left, // ウィンドウ横幅
		wrc.bottom - wrc.top, // ウィンドウ縦幅
		nullptr, // 親ウィンドウハンドル
		nullptr, // メニューハンドル
		w.hInstance, // 呼び出しアプリケーションハンドル
		nullptr); // オプション
		// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	// DirectX 初期化処理 ここから
	HRESULT result;
	dev = nullptr;

	// DXGI ファクトリーの生成
#ifdef _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
#endif
	// アダプターの列挙用
	std::vector< ComPtr<IDXGIAdapter>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr < IDXGIAdapter> tmpAdapter = nullptr;
	for (int i = 0;
		dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter); // 動的配列に追加する
	}
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		adapters[i]->GetDesc(&adesc); // アダプターの情報を取得
		std::wstring strDesc = adesc.Description; // アダプター名
		// Microsoft Basic Render Driver を回避
		if (strDesc.find(L"Microsoft") == std::wstring::npos && strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; // 採用
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
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(dev.ReleaseAndGetAddressOf()));
		if (result == S_OK)
		{
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	// コマンドアロケータを生成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdAllocator.ReleaseAndGetAddressOf()));
	// コマンドリストを生成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf()));

	// 標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf()));

	// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapchainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapchainDesc.BufferCount = 2; // バッファ数を2つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1 のComPtrを用意
	ComPtr<IDXGISwapChain1> swapchain1;
	//スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);
	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapchain1.As(&swapchain);

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);

	// 各種設定をしてデスクリプタヒープを生成
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	heapDesc.NumDescriptors = 2; // 裏表の2つ
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps.ReleaseAndGetAddressOf()));
	backBuffers.resize(swcDesc.BufferCount);
	// 裏表の2つ分について
	for (int i = 0; i < 2; i++)
	{
		// スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffers[i].ReleaseAndGetAddressOf()));
		// ディスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i,
				dev.Get()->GetDescriptorHandleIncrementSize(heapDesc.Type));
		// レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);

	}

	// フェンスの生成
	fenceVal = 0;
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));

	//深度バッファの作成
	//リソース設定
	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32_TYPELESS,
		DX12Util::GetWindowWidth(),
		DX12Util::GetWindowHeight(),
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//リソース生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer[0])
	);
	//リソース生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer[1])
	);


	//コマンド発行のために深度ビューを生成する
	//深度ビュー用デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapdesc{};
	dsvHeapdesc.NumDescriptors = 2;	//深度ビューは1つ
	dsvHeapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー
	result = DX12Util::GetDevice()->CreateDescriptorHeap(
		&dsvHeapdesc,
		IID_PPV_ARGS(dsvHeap.ReleaseAndGetAddressOf())
	);

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DX12Util::GetDevice()->CreateDepthStencilView(
		depthBuffer[0].Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//深度ビュー作成
	DX12Util::GetDevice()->CreateDepthStencilView(
		depthBuffer[1].Get(),
		&dsvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			dsvHeap->GetCPUDescriptorHandleForHeapStart(), 1, DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV))
	);


	//ImgUi初期化
	ImguiHelper::Initialize();

	//FbxLoader初期化
	FbxLoader::GetInstance()->Initialize(DX12Util::GetDevice());

	//インプット初期化
	Input::Initialize(DX12Util::GetHwnd());

	//サウンド(XAudio2初期化)
	Sound::StaticInitialize(true);

	//デバイスセット
	Object3D::SetDevice(DX12Util::GetDevice());

	//ヒープ生成
	Object3D::StaticInitialize();

	//スプライト初期化
	Sprite::FirstInit();

	//文字描画クラス初期化
	RenderText::StaticInitialize();

	//デバッグテキスト初期化
	DebugText::Initialize(L"Resources/System/debugfont.png");

	//パーティクル初期化
	ParticleManager::StaticInitialize();
	Particle3D::StaticInitialize();
	Particle2D::StaticInitialize();

}

void DX12Util::PreDrawBB()
{
	// バックバッファの番号を取得(2 つなので 0 番か 1 番)
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	// 1.リソースバリアを変更
	//表示状態から描画状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 2-1.レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	
	//2-2.深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//レンダーターゲットセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// 3.画面クリアコマンドここから

	// 全画面クリア R G B A		
	float clearColor[4] = { 0.1,0.25,0.5};
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	// 3.画面クリアコマンドここまで

	//4.ビューポートの設定コマンド
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, window_width, window_height));
	//5.シザー矩形の設定コマンド
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, window_width, window_height));
}

void DX12Util::PostDrawBB()
{
	// バックバッファの番号を取得(2 つなので 0 番か 1 番)
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//描画状態から表示状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//11.命令のクローズ
	cmdList->Close();

	//12.コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; // コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	//13.バッファをフリップ(裏表の入替え)
	swapchain->Present(1, 0);

	//14.コマンドリストの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//15.コマンドアロケーターのリセット
	cmdAllocator->Reset(); // キューをクリア

	//16.コマンドリストのリセット
	cmdList->Reset(cmdAllocator.Get(), nullptr); // 再びコマンドリストを貯める準備
}

void DX12Util::End()
{
	Sound::StaticFinalize();
	SceneManager::DeleteScene();
	FbxLoader::GetInstance()->Finalize();
	ImguiHelper::Finalize();

	//コマンドリストの実行完了を待つ
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
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart(), isShadow, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}


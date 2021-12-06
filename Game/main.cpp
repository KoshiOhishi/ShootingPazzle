#include <Windows.h>
#include <wrl.h>
#include "Input.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Sound.h"
#include "DX12Util.h"
#include "SceneManager.h"
#include "FPSManager.h"
#include "GamePlay.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "Timer.h"
#include "ImguiHelper.h"
#include "Object3D.h"
#include "RenderText.h"
#include "GameUtility.h"

using namespace DirectX;
using namespace Microsoft::WRL;

HRESULT result;
//ポストエフェクト
PostEffect* postEffect = nullptr;
//タイマー
Timer* timer;
//デバッグレイヤーをオンに
ComPtr<ID3D12Debug> debugController;
ComPtr<ID3D12DebugDevice> mDebugDevice;

//プロトタイプ宣言
void Initialize();
void Update();
void Finalize();

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//初期化
	Initialize();

	//シーンセット
	SceneManager::AddScene(new Editor(), "GamePlay");
	SceneManager::SetScene("GamePlay");

	//タイマー
	timer = new Timer(0, INT_MAX);
	timer->Start();

	MSG msg{}; // メッセージ
	while (true)
	{
		// メッセージがある?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg); // プロシージャにメッセージを送る
		}
		// 終了メッセージが来たらループを抜ける
		if (msg.message == WM_QUIT) {
			break;
		}

		Update();
	}

	Finalize();

	return 0;
}

void Initialize(){
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.ReleaseAndGetAddressOf()))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	//各種初期化
	DX12Util::Initialize(L"Game", 1280, 720);

	//ImgUi初期化
	ImguiHelper::Initialize();

#ifdef _DEBUG
	DX12Util::GetDevice()->QueryInterface(mDebugDevice.GetAddressOf());
#endif

	//FbxLoader初期化
	FbxLoader::GetInstance()->Initialize(DX12Util::GetDevice());

	//インプット初期化
	Input::Initialize(DX12Util::GetHwnd());

	//サウンド(XAudio2初期化)
	Sound::StaticInitialize(true);

	//デバイスセット
	Object3D::SetDevice(DX12Util::GetDevice());

	//ヒープ生成
	Object3D::CreateDescHeap();
#pragma region FBXパイプライン生成
	{
		//グラフィックパイプライン生成
		DX12Util::PipelineData pipelineData;
		pipelineData.vertexShaderFileName = L"Shader/FBXVS.hlsl";
		pipelineData.pixelShaderFileName = L"Shader/FBXPS.hlsl";
		pipelineData.inputLayout.resize(5);
		pipelineData.inputLayout[0] =
		{
			// xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[1] =
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[2] =
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[3] =
		{ // スキンインデックス
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[4] =
		{ // スキンウェイト
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		//デスクリプタレンジ
		pipelineData.descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		//ルートパラメータ
		pipelineData.rootparams.resize(6);
		// CBV（座標変換行列用）
		pipelineData.rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		pipelineData.rootparams[1].InitAsDescriptorTable(1, &pipelineData.descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（モデル用）
		pipelineData.rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（ライト用）
		pipelineData.rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（スキニング用）
		pipelineData.rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（マテリアル用）
		pipelineData.rootparams[5].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);
		//パイプライン生成
		Object3D::CreateGraphicsPipeline(Object3D::ObjectType::OBJECTTYPE_FBX, pipelineData);
	}
#pragma endregion
#pragma region OBJパイプライン生成
	{
		//グラフィックパイプライン生成
		DX12Util::PipelineData pipelineData;
		pipelineData.vertexShaderFileName = L"Shader/OBJVS.hlsl";
		pipelineData.geometryShaderFileName = L"Shader/OBJGS.hlsl";
		pipelineData.pixelShaderFileName = L"Shader/OBJPS.hlsl";
		pipelineData.inputLayout.resize(3);
		pipelineData.inputLayout[0] =
		{
			// xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[1] =
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		pipelineData.inputLayout[2] =
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		//デスクリプタレンジ
		pipelineData.descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		//ルートパラメータ
		pipelineData.rootparams.resize(4);
		// CBV（座標変換行列用）
		pipelineData.rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		pipelineData.rootparams[1].InitAsDescriptorTable(1, &pipelineData.descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（モデル用）
		pipelineData.rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（ライト用）
		pipelineData.rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		//パイプライン生成
		Object3D::CreateGraphicsPipeline(Object3D::ObjectType::OBJECTTYPE_OBJ, pipelineData);
	}
#pragma endregion
	//オブジェクト管理クラス初期化
	Object3DManager::Initialize();

	//スプライト初期化
	Sprite::FirstInit();

	//文字描画クラス初期化
	RenderText::StaticInitialize();

	//デバッグテキスト初期化
	DebugText::Initialize(L"Resources/System/debugfont.png");

	//ポストエフェクトの初期化
	postEffect = new PostEffect();
	postEffect->Initialize();

	//ゲーム静的初期化
	GameUtility::StaticInitialize();

}
void Update() {

	//FPSManager::Initialize(60);
	//インプット更新
	Input::Update();

	timer->Update();
	postEffect->SetTime((float)timer->GetNowTime());
	//DebugText::Print(std::to_string((float)timer->GetNowTime() / 1000), 0, 0);

	//シーン更新
	SceneManager::Update();
	//オブジェクト一括更新
	Object3DManager::UpdateAllObject();

	//レンダーテクスチャへの描画
	postEffect->PreDrawScene();
	SceneManager::Draw();
	postEffect->PostDrawScene();

	//描画開始前処理
	DX12Util::BeginDraw();

	//ポストエフェクトの描画
	postEffect->Draw();

	//デバッグテキストの描画
	DebugText::DrawAll();
	//描画終了処理
	DX12Util::EndDraw();

	//FPSを調整
	//FPSManager::AdjustFPS();

}
void Finalize() {
	//各種解放処理
	Sound::StaticFinalize();
	Object3DManager::DeleteAllObject();
	SceneManager::DeleteScene();
	FbxLoader::GetInstance()->Finalize();
	delete postEffect;
	ImguiHelper::Finalize();
	DX12Util::End();

#ifdef _DEBUG
	mDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY);
#endif
}
#include <Windows.h>
#include <wrl.h>
#include "Input.h"
#include "DebugText.h"
#include "DX12Util.h"
#include "SceneManager.h"
#include "FPSManager.h"
#include "Editor.h"
#include "PostEffect.h"
#include "Timer.h"
#include "Object3D.h"
#include "InstancingObject.h"
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
	SceneManager::AddScene(new Editor(), "Editor");
	SceneManager::ChangeScene("Editor");

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

void Initialize() {
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.ReleaseAndGetAddressOf()))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	//各種初期化
	DX12Util::Initialize(L"Stage Editor", 1280, 720);

#ifdef _DEBUG
	DX12Util::GetDevice()->QueryInterface(mDebugDevice.GetAddressOf());
#endif

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
		pipelineData.descRanges.resize(2);
		pipelineData.descRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
		pipelineData.descRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ

		//ルートパラメータ
		pipelineData.rootparams.resize(8);
		// CBV（共有情報用）
		pipelineData.rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（座標変換行列用）
		pipelineData.rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（モデル用）
		pipelineData.rootparams[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（ライト用）
		pipelineData.rootparams[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（スキニング用）
		pipelineData.rootparams[4].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（マテリアル用）
		pipelineData.rootparams[5].InitAsConstantBufferView(5, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ用）
		pipelineData.rootparams[6].InitAsDescriptorTable(1, &pipelineData.descRanges[0], D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		pipelineData.rootparams[7].InitAsDescriptorTable(1, &pipelineData.descRanges[1], D3D12_SHADER_VISIBILITY_ALL);
		//パイプライン生成
		Object3D::CreateGraphicsPipeline(ObjectType::OBJECTTYPE_FBX, pipelineData);

		//シェーダ変えてインスタンシングも
		pipelineData.vertexShaderFileName = L"Shader/InstancingFBXVS.hlsl";
		pipelineData.pixelShaderFileName = L"Shader/InstancingFBXPS.hlsl";
		InstancingObjectDraw::CreateGraphicsPipeline(ObjectType::OBJECTTYPE_INSTANCING_FBX, pipelineData);
	}
#pragma endregion
#pragma region OBJパイプライン生成
	{
		//グラフィックパイプライン生成
		DX12Util::PipelineData pipelineData;
		pipelineData.vertexShaderFileName = L"Shader/OBJVS.hlsl";
		//pipelineData.geometryShaderFileName = L"Shader/OBJGS.hlsl";
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
		pipelineData.descRanges.resize(2);
		pipelineData.descRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
		pipelineData.descRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ

		//ルートパラメータ
		pipelineData.rootparams.resize(6);
		// CBV（共有情報用）
		pipelineData.rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（座標変換行列用）
		pipelineData.rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（モデル用）
		pipelineData.rootparams[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		// CBV（ライト用）
		pipelineData.rootparams[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		pipelineData.rootparams[4].InitAsDescriptorTable(1, &pipelineData.descRanges[0], D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		pipelineData.rootparams[5].InitAsDescriptorTable(1, &pipelineData.descRanges[1], D3D12_SHADER_VISIBILITY_ALL);
		//パイプライン生成
		Object3D::CreateGraphicsPipeline(ObjectType::OBJECTTYPE_OBJ, pipelineData);

		//シェーダ変えてインスタンシングも
		pipelineData.vertexShaderFileName = L"Shader/InstancingOBJVS.hlsl";
		pipelineData.pixelShaderFileName = L"Shader/InstancingOBJPS.hlsl";
		pipelineData.geometryShaderFileName = nullptr;// L"Shader/InstancingOBJGS.hlsl";
		InstancingObjectDraw::CreateGraphicsPipeline(ObjectType::OBJECTTYPE_INSTANCING_OBJ, pipelineData);
	}

	Object3D::CreateShadowObjGraphicsPipeline();
	Object3D::CreateShadowFbxGraphicsPipeline();
	InstancingObjectDraw::CreateShadowObjGraphicsPipeline();
	InstancingObjectDraw::CreateShadowFbxGraphicsPipeline();

#pragma endregion

	//ポストエフェクトの初期化
	postEffect = new PostEffect();
	postEffect->Initialize();

	//ゲーム静的初期化
	GameUtility::StaticInitialize();

	FPSManager::SetFPS(0, false);

}
void Update() {

	//インプット更新
	Input::Update();

	timer->Update();
	postEffect->SetTime((float)timer->GetNowTime());
	//DebugText::Print(std::to_string((float)timer->GetNowTime() / 1000), 0, 0);

	//シーン更新
	SceneManager::Update();
	//オブジェクト一括更新
	Object3DManager::UpdateAllObject();

	//ペラポリゴンへの描画
	SceneManager::Draw();

	//バックバッファへの描画
	DX12Util::PreDrawBB();
	//シーンを描画したペラポリゴンを描画
	postEffect->Draw();
	//デバッグテキストの描画

	DebugText::DrawAll();
	DX12Util::PostDrawBB();

	//FPSを調整
	FPSManager::Update();
}
void Finalize() {
	//各種解放処理
	delete postEffect;
	DX12Util::End();
}
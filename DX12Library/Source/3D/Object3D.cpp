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
/// 静的メンバ変数の実体
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

	//デスクリプタヒープを生成
	// 設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダーから見える
	descHeapDesc.NumDescriptors = 32; // SRV1つ
	// 生成
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(basicDescHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Object3D::CreateGraphicsPipeline(int objectType, PipelineData& pipelineData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	//ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	assert(device);

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		pipelineData.vertexShaderFileName,    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ジオメトリシェーダの読み込みとコンパイル
	if (pipelineData.geometryShaderFileName != nullptr) {
		result = D3DCompileFromFile(
			pipelineData.geometryShaderFileName,    // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "gs_5_0",    // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		pipelineData.pixelShaderFileName,    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//// 頂点レイアウト
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//	{ // xy座標(1行で書いたほうが見やすい)
	//		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // 法線ベクトル(1行で書いたほうが見やすい)
	//		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	if (pipelineData.geometryShaderFileName != nullptr) {
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	}
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//透明部分の深度値書き込み禁止
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // 描画対象は2つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(pipelineData.rootparams.size(), pipelineData.rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	if (objectType == ObjectType::OBJECTTYPE_OBJ) {
		// ルートシグネチャの生成
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(objRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = objRootsignature.Get();

		// グラフィックスパイプラインの生成
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(objPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

	}
	else if (objectType == ObjectType::OBJECTTYPE_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(fbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = fbxRootsignature.Get();

		// グラフィックスパイプラインの生成
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(fbxPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }
	}


}

Object3D::~Object3D()
{
	if (collider) {
		//コリジョンマネージャから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

void Object3D::Initialize()
{
	HRESULT result;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataShare) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffShare.ReleaseAndGetAddressOf())
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffTransform.ReleaseAndGetAddressOf())
	);
	
	////オブジェクトマネージャーに追加
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//クラス名の文字列を取得
	name = typeid(*this).name();
}

void Object3D::Update()
{
	XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = XMMatrixIdentity();	//変形をリセット
	matWorld *= matScale;			//ワールド行列にスケーリングを反映
	matWorld *= matRot;				//ワールド行列に回転を反映
	matWorld *= matTrans;			//ワールド行列に平行移動を反映

	if (objectType == OBJECTTYPE_OBJ) {

		//ビュープロジェクション行列
		const XMMATRIX& matViewProjection = camera->GetViewProjection();
		//カメラ座標
		const Vector3& cameraPos = camera->GetPosition();

		HRESULT result;

		//定数バッファへデータ転送
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

		//ビュープロジェクション行列
		const XMMATRIX& matViewProjection = camera->GetViewProjection();
		//モデルのメッシュトランスフォーム
		const XMMATRIX& modelTransform = fbxModel->GetModelTransForm();
		//カメラ座標
		const Vector3& cameraPos = camera->GetPosition();

		HRESULT result;

		//定数バッファへデータ転送
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

		//アニメーション
		if (isPlay) {
			//1フレーム進める
			currentTime += frameTime;
			//最後まで再生したら先頭に戻す
			if (currentTime > endTime) {
				currentTime = startTime;
			}
		}

		//ボーン配列
		std::vector<FbxModel::Bone>& bones = fbxModel->GetBones();

		//定数バッファへデータ転送
		ConstBufferDataSkin* constMapSkin = nullptr;
		result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
		for (int i = 0; i < bones.size(); i++) {
			//今の姿勢行列
			XMMATRIX matCurrentPose;
			//今の姿勢行列を取得
			FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			//XMMATRIXに変換
			FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
			//合成してスキニング行列に
			constMapSkin->bones[i] = fbxModel->GetModelTransForm() * bones[i].invInitialPose * matCurrentPose * fbxModel->GetModelTransForm();
		}
		constBuffSkin->Unmap(0, nullptr);

	}

	//当たり判定更新
	if (collider) {
		collider->Update();
	}
}

void Object3D::Draw()
{
	if (objectType == OBJECTTYPE_OBJ) {
		//モデルの割り当てがなければ描画しない
		if (objModel == nullptr) {
			return;
		}

		//パイプラインステートの設定
		DX12Util::GetCmdList()->SetPipelineState(objPipelinestate.Get());
		//ルートシグネチャの設定
		DX12Util::GetCmdList()->SetGraphicsRootSignature(objRootsignature.Get());

		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//ライトの描画
		light->Draw(3);
		//モデル描画
		objModel->Draw();

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//モデルの割り当てがなければ描画しない
		if (fbxModel == nullptr) {
			return;
		}

		//パイプラインステートの設定
		DX12Util::GetCmdList()->SetPipelineState(fbxPipelinestate.Get());
		//ルートシグネチャの設定
		DX12Util::GetCmdList()->SetGraphicsRootSignature(fbxRootsignature.Get());
		//プリミティブ形状を設定
		DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
		//ライトの描画
		light->Draw(3);
		//モデル描画
		fbxModel->Draw();
	}


}

void Object3D::PlayAnimation()
{
	if (objectType == OBJECTTYPE_OBJ) {
		return;
	}

	FbxScene* fbxScene = fbxModel->GetFbxScene();
	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	//アニメーションの時間取得
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime = startTime;
	//再生中状態にする
	isPlay = true;
}

void Object3D::SetFbxModel(FbxModel* fbxModel)
{
	this->fbxModel = fbxModel;

	//定数バッファの生成
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffSkin.ReleaseAndGetAddressOf())
	);

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < MAX_BONES; i++) {
			constMapSkin->bones[i] = XMMatrixIdentity();
		}
		constBuffSkin->Unmap(0, nullptr);
	}

	//1フレーム分の時間を60FPSで設定
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
	//既に登録されている場合はRemove
	if (this->collider) {
		CollisionManager::GetInstance()->RemoveCollider(this->collider);
	}

	collider->SetObject(this);
	this->collider = collider;
	//コリジョンマネージャに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//コライダーを更新しておく
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

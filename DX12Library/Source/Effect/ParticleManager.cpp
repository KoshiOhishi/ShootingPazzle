#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "DX12Util.h"
#include "DrawManager.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
UINT ParticleManager::descriptorHandleIncrementSize = 0;
ComPtr<ID3D12DescriptorHeap> ParticleManager::descHeap;
const int ParticleManager::PATICLE_SRV_COUNT = 512;
ComPtr<ID3D12Resource> ParticleManager::texbuff[ParticleManager::PATICLE_SRV_COUNT];
CD3DX12_CPU_DESCRIPTOR_HANDLE ParticleManager::cpuDescHandleSRV;
CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleManager::gpuDescHandleSRV;
UINT ParticleManager::loadedTextureCount = 0;
std::unordered_map<std::wstring, UINT> ParticleManager::loadedTextureList;

void ParticleManager::StaticInitialize()
{
	HRESULT result = S_FALSE;
	ID3D12Device* device = DX12Util::GetDevice();

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 512; // シェーダーリソースビュー1つ
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	loadedTextureList.clear();
}

void ParticleManager::LoadTexture(const std::wstring& filename)
{
	//すでに読み込んである場合はそのテクスチャを使う
	if (loadedTextureList.find(filename) != loadedTextureList.end())
	{
		texNumber = loadedTextureList.find(filename)->second;
		return;
	}

	ID3D12Device* device = DX12Util::GetDevice();

	HRESULT result = S_FALSE;

	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename.c_str(), WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// テクスチャ用バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texbuff[loadedTextureCount].ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}

	// テクスチャバッファにデータ転送
	result = texbuff[loadedTextureCount]->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels,    // 元データアドレス
		(UINT)img->rowPitch,  // 1ラインサイズ
		(UINT)img->slicePitch // 1枚サイズ
	);
	if (FAILED(result)) {
		assert(0);
	}

	// シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff[loadedTextureCount]->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texbuff[loadedTextureCount].Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), 
			loadedTextureCount, 
			descriptorHandleIncrementSize)
	);

	loadedTextureList.emplace(filename, loadedTextureCount);

	texNumber = loadedTextureCount;

	loadedTextureCount++;
}
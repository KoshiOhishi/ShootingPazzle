#pragma once
#include "Sprite.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#pragma comment(lib, "d3d12.lib")

class Shadow:
	public Sprite
{
protected: //エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//std::を省略
	using string = std::string;

private:
	static const float clearColor[4];

	//SRV用ヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDepthSRV;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

public:
	void Initialize();
	void Draw();

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	void PostDrawScene();

	//VBV作成
	void CreateVBV();
	//定数バッファ生成
	void CreateConstBuffer();
	//テクスチャ生成
	void CreateTexture();
	//SRV作成
	void CreateSRV();
	//RTV作成
	void CreateRTV();
	//DSV作成
	void CreateDSV();
	//パイプライン生成
	void CreateGraphicsPipelineState();
};


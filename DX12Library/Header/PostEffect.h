#pragma once
#include "Sprite.h"

class PostEffect : 
	public Sprite
{
public: //サブクラス
	struct ConstBufferDataTime 
	{
		float time;
	};

private:
	static const int RENDERCOUNT = 2;
	static const float clearColor[4];

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(bool isDispDepthTex = false);

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	static void PreDrawScene();

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	static void PostDrawScene();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw();

private:
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
	//パイプライン生成
	void CreateGraphicsPipelineState(bool isDispDepthTex = false);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetRtvH(int targetNum);

private: //メンバ変数
	//テクスチャバッファ
	static ComPtr<ID3D12Resource> texBuff[RENDERCOUNT];
	//RTV用デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeapRTV;

	//定数バッファ（時間）
	ComPtr<ID3D12Resource> constBuffTime;
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//時間
	float time;

public:

	void SetTime(float time) { PostEffect::time = time; }
};


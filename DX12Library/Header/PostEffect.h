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
	static const int RENDERCOUNT = 1;
	static const float clearColor[4];

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	void PostDrawScene();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	void Draw();

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

private: //メンバ変数

	//定数バッファ（時間）
	ComPtr<ID3D12Resource> constBuffTime;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff[RENDERCOUNT];
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//時間
	float time = 0;

public:

	void SetTime(float time) { this->time = time; }
};


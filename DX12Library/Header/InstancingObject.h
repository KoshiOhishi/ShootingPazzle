#pragma once
#include "Object3D.h"

class InstancingObject :
    public Object3D
{
protected:
	//FBXルートシグネチャ
	static ComPtr<ID3D12RootSignature> instancingFbxRootsignature;
	//FBXパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> instancingFbxPipelinestate;

	//OBJルートシグネチャ
	static ComPtr<ID3D12RootSignature> instancingObjRootsignature;
	//OBJパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> instancingObjPipelinestate;

public:
	static const int MAX_INSTANCECOUNT = 512;

	struct ConstBufferDataInstancing
	{
		InstanceData data[MAX_INSTANCECOUNT];
	};

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	/// <param name="objectType">生成するパイプラインが使用されるオブジェクトの種類</param>
	/// <param name="pipelineData">パイプライン設定変数</param>
	static void CreateGraphicsPipeline(int objectType, PipelineData& pipelineData);

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() override;

	/// <summary>
	/// 毎フレーム処理　1回のみ呼び出し
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画　1回のみ呼び出し
	/// </summary>
	virtual void Draw() override;

	/// <summary>
	/// バッファ更新　こちらをループさせる
	/// </summary>
	/// <param name="index">インデックス</param>
	void UpdateBuffer(int index);
};


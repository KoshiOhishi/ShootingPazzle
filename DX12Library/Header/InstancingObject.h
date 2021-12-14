#pragma once
#include "Object3D.h"

static const int MAX_INSTANCECOUNT = 512;

//モデルにつき1つ
class InstancingObjectDraw :
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

	int index = 0;
public:

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
	virtual void Initialize();

	/// <summary>
	/// 毎フレーム処理　1回のみ呼び出し
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画　1回のみ呼び出し
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 定数バッファに書き込み
	/// </summary>
	/// <param name="constBuffData"></param>
	void UpdateConstBuff(const InstanceData& constBuffData);

	/// <summary>
	/// 描画関数を通らないときはこれを書く
	/// </summary>
	void ResetIndex() { index = 0; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetConstBuff() { return constBuffTransform; }
};

//各個体に1つ
class InstancingObject
{
private:

	//ローカルスケール
	Vector3 scale = { 1,1,1 };
	//X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0,0,0 };
	//ローカル座標
	Vector3 position = { 0,0,0 };
	//色
	Vector4 color = { 1,1,1,1 };
	//ローカルワールド変換行列
	DirectX::XMMATRIX matWorld;

public:
	void Initialize(const Vector3& pos = {}, const Vector3& rot = {}, const Vector3& scale = {1,1,1}, const Vector4& color = {1,1,1,1});
	void Update(InstancingObjectDraw& instancingObjectDraw);

	void SetPosition(const Vector3& pos) { position = pos; }
	void SetRotation(const Vector3& rot) { rotation = rot; }
	void SetScale(const Vector3& scale) { this->scale = scale; }
	void SetColor(const Vector4& color) { this->color = color; }

	const Vector3& GetPosition() { return position; }
	const Vector3& GetRotation() { return rotation; }
	const Vector3& GetScale() { return scale; }
	const Vector4& GetColor() { return color; }
};
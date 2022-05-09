#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

class Tutorial
{
private:
	//ファイルディレクトリ
	static const std::string DIR;
	//位置調整
	static const float ADJUST_POS;

	//テクスチャ
	DX12Library::Sprite popup;
	DX12Library::Sprite mouse;
	DX12Library::Sprite mouseCover;
	DX12Library::Sprite text;

	//基準座標
	DX12Library::Vector2 masterPos;

	//タイマー
	DX12Library::Timer moveMasterPosTimer;
	DX12Library::Timer roopEffectTimer;
	DX12Library::Timer moveOverlapMouseTimer;

	//チュートリアル有効か
	bool isEnable = false;
	//移動の向きを逆にするか
	bool reverseMove = false;
	//最初のエフェクト終了フラグ
	bool isEndFirstEffect = false;

public:
	Tutorial();
	~Tutorial(){}

	void Initialize(bool isEnable);
	void Update();
	void Draw();

	void UpdateTexFromPhase();
	void UpdateTexPos();
	void UpdateTexPosOverlapMouse(DX12Library::Vector2& masterPos);

	//マウスとチュートリアルが重なっているか
	bool IsOverlapMouse();
};
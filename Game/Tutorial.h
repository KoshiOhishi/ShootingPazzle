#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

class Tutorial
{
private:
	//ファイルディレクトリ
	static const std::wstring DIR;
	//位置調整
	static const float ADJUST_POS;

	//テクスチャ
	Sprite popup;
	Sprite mouse;
	Sprite mouseCover;
	Sprite text;

	//基準座標
	Vector2 masterPos;

	//タイマー
	Timer moveMasterPosTimer;
	Timer roopEffectTimer;

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
};
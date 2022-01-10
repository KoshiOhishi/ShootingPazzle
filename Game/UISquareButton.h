#pragma once
#include "Sprite.h"
#include <string>

/// <summary>
/// 四角形ボタンUIクラス
/// </summary>
class UISquareButton
{
private:
	//ボタンテクスチャ
	Sprite textureOn;
	Sprite textureOff;

	Vector2 pos;
	Vector2 size;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="texturePath">画像パス 画像は左半分にoff、右半分にon時の画像をつなげたものを使用/param>
	/// <param name="pos">座標</param>
	void Initialize(const std::wstring& texturePath, const Vector2& pos);

	void Draw();

	/// <summary>
	/// マウスカーソルがボタンに重なっているかを返す
	/// </summary>
	/// <returns></returns>
	bool IsOverlapMouseCursol();

	/// <summary>
	/// ボタンが押されているかを返す
	/// </summary>
	/// <returns></returns>
	bool IsPushButton();

	/// <summary>
	/// ボタンを押した瞬間か返す
	/// </summary>
	/// <returns></returns>
	bool IsTriggerButton();

	/// <summary>
	/// ボタンが押された状態から離されたかどうか返す
	/// </summary>
	/// <returns></returns>
	bool IsReleaseButton();

	void SetPosition(const Vector2& pos);

	const Vector2& GetPosition() { return pos; }

	const Vector2& GetSize() { return size; }
};


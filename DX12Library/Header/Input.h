#pragma once
#define DIRECTINPUT_VERSION	0x0800	//DirectInputのバージョン指定

#include <Windows.h>
#include <wrl.h>
#include "Vector2.h"
#include "Vector3.h"
#include <dinput.h>
#include "CollisionPrimitive.h"
#include "Camera.h"

enum MouseButton
{
	LEFT,
	RIGHT,
	MIDDLE
};

class Input
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static ComPtr<IDirectInput8> dinput;

public:
	static void Initialize(HWND hwnd);
	static void Update();
};

class Keyboard
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static BYTE key[256];
	static BYTE prevKey[256];
	static ComPtr<IDirectInputDevice8> devkeyboard;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hwnd"></param>
	static void Initialize(HWND hwnd, IDirectInput8* dinput);

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// キー押下状況取得(トリガー)
	/// </summary>
	/// <param name="keynum">キー番号(DIK_)</param>
	/// <returns>キー押下状況</returns>
	static bool IsKeyTrigger(int keynum);

	/// <summary>
	/// キー押下状況取得
	/// </summary>
	/// <param name="keynum">キー番号(DIK_)</param>
	/// <returns>キー押下状況</returns>
	static bool IsKeyPush(int keynum);

};

class Mouse
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static DIMOUSESTATE2 mouseState;
	static DIMOUSESTATE2 prevMouseState;
	static ComPtr<IDirectInputDevice8> devMouse;
	static Ray ray;
	static Camera* pCamera;

private:
	/// <summary>
	/// スクリーン座標からワールド座標に変換する
	/// </summary>
	/// <param name="fZ"></param>
	/// <returns></returns>
	static const Vector3& CalcPosScreenToWorld(float fZ);

public:
	//メンバ関数の宣言

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hwnd"></param>
	static void Initialize(HWND hwnd, IDirectInput8* dinput);

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// マウスボタン押下状況取得(トリガー)
	/// </summary>
	/// <param name="mouseButtonNum">マウスボタン番号(MouseButton::)</param>
	/// <returns>マウスボタン押下状況</returns>
	static bool IsMouseButtonTrigger(int mouseButtonNum);

	/// <summary>
	/// マウスボタン押下状況取得(離された瞬間)
	/// </summary>
	/// <param name="mouseButtonNum"></param>
	/// <returns>マウスボタン押下状況</returns>
	static bool IsMouseButtonRelease(int mouseButtonNum);

	/// <summary>
	/// マウスボタン押下状況取得
	/// </summary>
	/// <param name="mouseButtonNum">マウスボタン番号(MouseButton::)</param>
	/// <returns>マウスボタン押下状況</returns>
	static bool IsMouseButtonPush(int mouseButtonNum);

	/// <summary>
	/// マウス1フレーム移動量
	/// </summary>
	/// <returns>移動量</returns>
	static const Vector2& GetMouseMove();

	/// <summary>
	/// マウス1フレームホイール移動量
	/// </summary>
	/// <returns>ホイール移動量</returns>
	static float GetMouseWheelMove();

	/// <summary>
	/// マウス座標取得
	/// </summary>
	/// <param name="isRelative">ウィンドウ左上を0とするか</param>
	/// <returns>座標</returns>
	static const Vector2& GetMousePos(bool isRelative = true);

	/// <summary>
	/// マウス座標セット
	/// </summary>
	static void SetMousePos(const Vector2& pos, bool isRelative = true);

	/// <summary>
	/// カメラをセット
	/// </summary>
	/// <param name="pCamera">カメラポインタ</param>
	static void SetCamera(Camera* pCamera) { Mouse::pCamera = pCamera; }

	/// <summary>
	/// マウスと3Dオブジェクトの当たり判定に使用するレイを返す
	/// </summary>
	/// <returns>レイ</returns>
	static const Ray& GetMouseRay() { return ray; }
};

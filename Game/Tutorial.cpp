#include "Tutorial.h"
#include "Easing.h"
#include "DX12Util.h"
#include "DebugText.h"
#include "GameUtility.h"
#include "Input.h"

const std::wstring Tutorial::DIR = L"Resources/Tutorial/";
const float Tutorial::ADJUST_POS = 10;

Tutorial::Tutorial()
{
	//画像読み込み
	popup.Initialize();
	popup.SetTexture(DIR + L"Popup.png");
	mouse.Initialize();
	mouse.SetTexture(DIR + L"Mouse.png");
	mouseCover.Initialize();
	mouseCover.SetTexture(DIR + L"MouseCover.png");
	text.Initialize();
	text.SetTexture(DIR + L"Text.png");
}

void Tutorial::Initialize(bool isEnable)
{
	//フラグセット
	this->isEnable = isEnable;

	//基準座標セット
	masterPos = { (float)DX12Util::GetWindowWidth(), (float)DX12Util::GetWindowHeight() - ADJUST_POS - popup.GetTexSize().y };

	//テクスチャ初期位置セット
	popup.SetPosition(masterPos);
	mouse.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 124));
	mouseCover.SetPosition(mouse.GetPosition());
	text.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - text.GetTexSize().x / 4, 12));
	text.SetDrawRectangle(0,0,192,64);

	//タイマー初期化
	moveMasterPosTimer.SetTimer(0, 500);
	roopEffectTimer.SetTimer(0, 4000, true);
	roopEffectTimer.Start();
	moveOverlapMouseTimer.SetTimer(0, 500);
	moveOverlapMouseTimer.SetNowTime(moveOverlapMouseTimer.GetEndTime());

	reverseMove = false;
	isEndFirstEffect = false;
}

void Tutorial::Update()
{
	//有効でなければリターン
	if (isEnable == false) {
		return;
	}

	UpdateTexFromPhase();

	UpdateTexPos();
}

void Tutorial::Draw()
{
	//有効でなければリターン
	if (isEnable == false) {
		return;
	}

	popup.DrawFG();
	mouse.DrawFG();
	mouseCover.DrawFG();
	text.DrawFG();
}

void Tutorial::UpdateTexPos()
{
	moveMasterPosTimer.Update();
	//下地移動中はタイマーを初期値に
	if (moveMasterPosTimer.GetIsStart()) {
		roopEffectTimer.SetNowTime(0);
	}
	roopEffectTimer.Update();

	//基本位置更新
	if (reverseMove) {
		masterPos.x = Easing::GetEaseValue(EASE_OUTQUINT, (double)DX12Util::GetWindowWidth() - ADJUST_POS - popup.GetTexSize().x, (double)DX12Util::GetWindowWidth(), moveMasterPosTimer);
	}
	else {
		masterPos.x = Easing::GetEaseValue(EASE_OUTQUINT, (double)DX12Util::GetWindowWidth(), (double)DX12Util::GetWindowWidth() - ADJUST_POS - popup.GetTexSize().x, moveMasterPosTimer);
	}

	//マウスと重なった時の処理
	UpdateTexPosOverlapMouse(masterPos);

	int nowPhase = GameUtility::GetNowPhase();

	//フェーズ共通の位置更新
	popup.SetPosition(masterPos);
	text.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - text.GetTexSize().x / 4, 12));

	//フェーズごとの位置更新
	if (nowPhase == PHASE_GAME_FIRSTEFFECT ||
		nowPhase == PHASE_GAME_SETPOS) {
		//マウスの動き(横スイング)
		//中心位置
		const Vector2 CENTER = masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 124);
		//加算位置
		float rad = (float)roopEffectTimer.GetNowTime() * 360 / 2000 * PI / 180;
		Vector2 addPos = { sin(rad) * 50, 0 };

		//タイマーが2000未満の時は左右に動く、そうでないときは停止
		if (roopEffectTimer.GetNowTime() >= 2000) {
			addPos = {};
		}
		//適用
		mouse.SetPosition(CENTER + addPos);
		mouseCover.SetPosition(mouse.GetPosition());

		//テキスト描画範囲セット
		text.SetDrawRectangle(0, 0, 192, 64);
	}
	else {
		//マウスの動き(下中央を中心にスイング)
		//下中心位置
		const Vector2 CENTER = masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 250);
		//加算位置
		float rad = (float)roopEffectTimer.GetNowTime() * 360 / 2000 * PI / 180;
		Vector2 addPos = { sin(rad) * 30, -abs(cos(rad) * 20) - 130 };

		//タイマーが2000未満の時は左右に動く、そうでないときは停止
		if (roopEffectTimer.GetNowTime() >= 2000) {
			addPos = { 0,-20 - 130 };
		}

		//適用
		mouse.SetPosition(CENTER + addPos);
		mouseCover.SetPosition(mouse.GetPosition());

		//テキスト描画範囲セット
		text.SetDrawRectangle(192, 0, 192, 64);
	}

	//指定時間は左クリック点灯
	if (roopEffectTimer.GetNowTime() >= 2500 && roopEffectTimer.GetNowTime() < 3500) {
		float gb = Easing::GetEaseValue(EASE_INOUTSINE, 1, 0, roopEffectTimer, 2500, 3500);
		float a = Easing::GetEaseValue(EASE_INCUBIC, 1, 0, roopEffectTimer, 2500, 3500);
		mouseCover.SetColor({ 1,gb,gb, a });
	}
	else {
		mouseCover.SetColor({ 1, 1, 1, 0 });
	}
}

void Tutorial::UpdateTexPosOverlapMouse(Vector2& masterPos)
{
	static bool prevOverlap = false;
	//マウスとチュートリアルが重なってない状態から重なったり
	//重なった状態から重なっていない状態になったらタイマー開始
	if (prevOverlap != IsOverlapMouse()) {
		moveOverlapMouseTimer.Reset();
		moveOverlapMouseTimer.Start();
	}

	moveOverlapMouseTimer.Update();

	//タイマーが動いていたらイージング使って値を動かす
	if (moveOverlapMouseTimer.GetIsStart()) {
		if (IsOverlapMouse()) {
			masterPos.x += Easing::GetEaseValue(EASE_OUTQUINT, 0, popup.GetTexSize().x + ADJUST_POS, moveOverlapMouseTimer);
		}
		else {
			masterPos.x += Easing::GetEaseValue(EASE_OUTQUINT, popup.GetTexSize().x + ADJUST_POS, 0, moveOverlapMouseTimer);
		}
	}
	else {
		if (IsOverlapMouse()) {
			masterPos.x += popup.GetTexSize().x + ADJUST_POS;
		}
	}

	prevOverlap = IsOverlapMouse();
}

void Tutorial::UpdateTexFromPhase()
{
	static int prevPhase = PHASE_GAME_FIRSTEFFECT;
	int nowPhase = GameUtility::GetNowPhase();
	if (nowPhase != prevPhase) {

		//射出するときは逆向きに移動
		if (nowPhase == PHASE_GAME_AFTERSHOOT) {
			reverseMove = true;

			moveMasterPosTimer.Reset();
			moveMasterPosTimer.Start();
		}
		//FIRSTEFFECT、アングル指定時、(最初のFIRSTEFFECTかつSETPOSフェーズのとき)には通常移動
		else if ((isEndFirstEffect == true && nowPhase == PHASE_GAME_FIRSTEFFECT) ||
			(isEndFirstEffect == false && nowPhase == PHASE_GAME_SETPOS) ||
			nowPhase == PHASE_GAME_SETANGLE) {
			reverseMove = false;
			if (isEndFirstEffect == false) {
				isEndFirstEffect = true;
			}

			moveMasterPosTimer.Reset();
			moveMasterPosTimer.Start();
		}
	}
	prevPhase = GameUtility::GetNowPhase();

	//シーン情報からスプライトテクスチャ更新

}
//最初のFIRSTEFFECTってややこしいからどうにかしたいなあ

bool Tutorial::IsOverlapMouse() {
	const Vector2 TUTORIAL_POS = {
	DX12Util::GetWindowWidth() - ADJUST_POS - popup.GetTexSize().x,
	DX12Util::GetWindowHeight() - ADJUST_POS - popup.GetTexSize().y
	};

	Vector2 mousePos = Mouse::GetMousePos();

	return mousePos.x >= TUTORIAL_POS.x && mousePos.x < DX12Util::GetWindowWidth() &&
		mousePos.y >= TUTORIAL_POS.y && mousePos.y < DX12Util::GetWindowHeight();
}
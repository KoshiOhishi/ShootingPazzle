#include "UISquareButton.h"
#include "Input.h"
#include "DebugText.h"
#include "Easing.h"
#include "GameSound.h"

void UISquareButton::LoadTexture(const std::wstring& texturePath)
{
    //画像読み込み
    textureOff.Initialize();
    textureOff.SetTexture(texturePath);
    textureOn.Initialize();
    textureOn.SetTexture(texturePath);
    textureAdd.Initialize();
    textureAdd.SetTexture(texturePath);
    //サイズ格納
    size = { textureOff.GetTexSize().x * 0.5f, textureOff.GetTexSize().y };
}

void UISquareButton::Initialize(const Vector2& pos)
{
    //Off
    textureOff.SetDrawRectangle(0, 0, size.x, size.y);
    textureOff.SetPosition(pos);
    //On
    textureOn.SetDrawRectangle(size.x, 0, size.x, size.y);
    textureOn.SetPosition(pos);

    //Add
    textureAdd.SetDrawRectangle(size.x, 0, size.x, size.y);
    textureAdd.SetPosition(pos + size * 0.5f);
    textureAdd.SetAnchorpoint({ 0.5f,0.5f });
    textureAdd.SetBlendMode(SPRITE_BLENDMODE_ADD);

    //位置を格納
    this->pos = pos;

    //タイマーは基本終了値にする
    pushedEffectTimer.SetTimer(0,1);
    pushedEffectTimer.Start();
}

void UISquareButton::Draw()
{
    //サウンド更新
    UpdateSound();

    //加算合成エフェクト
    pushedEffectTimer.Update();
    float alpha = Easing::GetEaseValue(EASE_OUTQUINT, 1, 0, pushedEffectTimer);
    float sizeX = Easing::GetEaseValue(EASE_OUTQUINT, size.x, size.x * 1.5, pushedEffectTimer);
    float sizeY = Easing::GetEaseValue(EASE_OUTQUINT, size.y, size.y * 1.5, pushedEffectTimer);
    Vector4 nowColor = textureAdd.GetColor();
    textureAdd.SetColor({ nowColor.x, nowColor.y, nowColor.z, alpha});
    textureAdd.SetSize({ sizeX,sizeY });

    if (IsOverlapMouseCursol() && isDispOverlapMouseTex) {
        textureOn.DrawFG();
    }
    else {
        textureOff.DrawFG();
    }

    if (pushedEffectTimer.GetIsEnd() == false) {
        textureAdd.DrawFG();
    }

}

void UISquareButton::StartPushedEffect()
{
    pushedEffectTimer.SetTimer(0, 1000);
    pushedEffectTimer.Start();
}

bool UISquareButton::IsOverlapMouseCursol()
{
    Vector2 mousePos = Mouse::GetMousePos();

    return mousePos.x >= pos.x && mousePos.x < pos.x + size.x &&
        mousePos.y >= pos.y && mousePos.y < pos.y + size.y;
}

bool UISquareButton::IsPushButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonPush(LEFT);
}

bool UISquareButton::IsTriggerButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonTrigger(LEFT);
}

bool UISquareButton::IsReleaseButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonRelease(LEFT);
}

void UISquareButton::UpdateSound()
{
    //選択音
    if (prevOverlap == false && IsOverlapMouseCursol()) {
        GameSound::Play(L"UI_Select");
    }

    //決定音
    if (IsReleaseButton()) {
        GameSound::Play(L"UI_Decide");
    }

    prevOverlap = IsOverlapMouseCursol();
}

void UISquareButton::SetPosition(const Vector2& pos)
{
    this->pos = pos;

    textureOff.SetPosition(pos);
    textureOn.SetPosition(pos);
    textureAdd.SetPosition(pos + size * 0.5f);
}

void UISquareButton::SetColor(const Vector4& color)
{
    textureOff.SetColor(color);
    textureOn.SetColor(color);
    textureAdd.SetColor(color);
}

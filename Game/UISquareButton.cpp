#include "UISquareButton.h"
#include "Input.h"
#include "DebugText.h"
#include "Easing.h"

void UISquareButton::Initialize(const std::wstring& texturePath, const Vector2& pos)
{
    //Off
    textureOff.Initialize();
    textureOff.SetTexture(texturePath);
    textureOff.SetPosition(pos);
    textureOff.SetDrawRectangle(0,0, textureOff.GetScale().x * 0.5, textureOff.GetScale().y);

    //On
    textureOn.Initialize();
    textureOn.SetTexture(texturePath);
    textureOn.SetPosition(pos);
    textureOn.SetDrawRectangle(textureOn.GetScale().x * 0.5, 0, textureOn.GetScale().x * 0.5, textureOn.GetScale().y);

    //位置とサイズを格納
    this->pos = pos;
    size = textureOff.GetScale();

    //タイマーは基本終了値にする
    pushedEffectTimer.SetTimer(0,1);
    pushedEffectTimer.Start();
}

void UISquareButton::Draw()
{
    //テクスチャの色更新
    pushedEffectTimer.Update();
    float color = Easing::GetEaseValue(EASE_OUTCUBIC, 6, 1, pushedEffectTimer);
    textureOn.SetColor({ color, color, color, 1 });
    textureOff.SetColor({ color, color, color, 1 });

    if (IsOverlapMouseCursol()) {
        textureOn.DrawFG();
    }
    else {
        textureOff.DrawFG();
    }

}

void UISquareButton::StartPushedEffect()
{
    pushedEffectTimer.SetTimer(0, 750);
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

void UISquareButton::SetPosition(const Vector2& pos)
{
    this->pos = pos;

    textureOff.SetPosition(pos);
    textureOn.SetPosition(pos);
}

#include "UISquareButton.h"
#include "Input.h"

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

    //ˆÊ’u‚ÆƒTƒCƒY‚ðŠi”[
    this->pos = pos;
    size = textureOff.GetScale();
}

void UISquareButton::Draw()
{
    if (IsOverlapMouseCursol()) {
        textureOn.DrawFG();
    }
    else {
        textureOff.DrawFG();
    }
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

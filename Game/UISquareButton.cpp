#include "UISquareButton.h"
#include "Input.h"
#include "DebugText.h"
#include "Easing.h"
#include "GameSound.h"

void UISquareButton::LoadTexture(const std::wstring& texturePath)
{
    //�摜�ǂݍ���
    textureOff.Initialize();
    textureOff.SetTexture(texturePath);
    textureOn.Initialize();
    textureOn.SetTexture(texturePath);
    textureAdd.Initialize();
    textureAdd.SetTexture(texturePath);
    //�T�C�Y�i�[
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

    //�ʒu���i�[
    this->pos = pos;

    //�^�C�}�[�͊�{�I���l�ɂ���
    pushedEffectTimer.SetTimer(0,1);
    pushedEffectTimer.Start();

    isEnable = true;
}

void UISquareButton::Draw()
{
    //�T�E���h�X�V
    if (isEnable) {
        UpdateSound();
    }

    //���Z�����G�t�F�N�g
    pushedEffectTimer.Update();
    float alpha = Easing::GetEaseValue(EASE_OUTQUINT, 1, 0, pushedEffectTimer);
    float sizeX = Easing::GetEaseValue(EASE_OUTQUINT, size.x, size.x * 1.5, pushedEffectTimer);
    float sizeY = Easing::GetEaseValue(EASE_OUTQUINT, size.y, size.y * 1.5, pushedEffectTimer);
    Vector4 nowColor = textureAdd.GetColor();
    textureAdd.SetColor({ nowColor.x, nowColor.y, nowColor.z, alpha});
    textureAdd.SetSize({ sizeX,sizeY });

    if (IsOverlapMouseCursol() && isEnable) {
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

    //�}�E�X����ʂ̓����ɂ��邩
    bool isInsideWindow =
        mousePos.x >= 0 && mousePos.x < DX12Util::GetWindowWidth() &&
        mousePos.y >= 0 && mousePos.y < DX12Util::GetWindowHeight();

    //�}�E�X���{�^���̓����ɂ��邩
    bool isInsideButton = 
        mousePos.x >= pos.x&& mousePos.x < pos.x + size.x &&
        mousePos.y >= pos.y && mousePos.y < pos.y + size.y;

    return isInsideWindow && isInsideButton;
}

bool UISquareButton::IsPushButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonPush(LEFT) && isEnable;
}

bool UISquareButton::IsTriggerButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonTrigger(LEFT) && isEnable;
}

bool UISquareButton::IsReleaseButton()
{
    return IsOverlapMouseCursol() && Mouse::IsMouseButtonRelease(LEFT) && isEnable;
}

void UISquareButton::UpdateSound()
{
    //�I����
    if (prevOverlap == false && IsOverlapMouseCursol()) {
        GameSound::Play(L"UI_Select");
    }

    //���艹
    if (IsReleaseButton()) {
        GameSound::Play(L"UI_Click");
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

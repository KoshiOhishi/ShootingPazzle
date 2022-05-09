#include "DebugText.h"
#include <string>

using namespace DX12Library;

//�ÓI�����o�ϐ��̎���
Sprite DebugText::sprites[maxCharCount];
int DebugText::spriteIndex = 0;
UINT DebugText::debugTextTexNumber;


void DebugText::Initialize(const std::string& texfilename)
{
	Sprite::LoadTexture(texfilename);
	for (int i = 0; i < _countof(sprites); i++)
	{
		sprites[i].Initialize();
		sprites[i].SetTexture(texfilename);
	}
}

void DebugText::SetDebugTextTexNumber(const int debugTextTexNumber)
{
	DebugText::debugTextTexNumber = (UINT)debugTextTexNumber;
}

void DebugText::Print(const std::string& text, float x, float y, float scale)
{
	//���ׂĂ̕����ɂ���
	for (int i = 0; i < text.size(); i++)
	{
		//�ő啶��������
		if (spriteIndex >= maxCharCount) {
			break;
		}

		//1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char& character = text[i];

		//ASCII�R�[�h��2�i����΂����ԍ����v�Z
		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//���W�v�Z
		sprites[spriteIndex].SetPosition(Vector2({x + fontWidth * scale * i, y}));
		sprites[spriteIndex].SetDrawRectangle(fontIndexX * fontWidth, fontIndexY * fontHeight, fontWidth, fontHeight);
		sprites[spriteIndex].SetSize(Vector2({fontWidth * scale, fontHeight * scale}));

		//������1�i�߂�
		spriteIndex++;

	}
}

void DebugText::Print(int text, float x, float y, float scale)
{
	Print(std::to_string(text), x, y, scale);
}

void DebugText::Print(float text, float x, float y, float scale)
{
	Print(std::to_string(text), x, y, scale);
}

void DebugText::Print(const Vector2& text, float x, float y, float scale)
{
	Print("(" + std::to_string(text.x) + ", " + std::to_string(text.y) + ")", x, y, scale);
}

void DebugText::Print(const Vector3& text, float x, float y, float scale)
{
	Print("(" + std::to_string(text.x) + ", " + std::to_string(text.y) + ", " + std::to_string(text.z) + ")", x, y, scale);
}

void DebugText::Print(const Vector4& text, float x, float y, float scale)
{
	Print("(" + std::to_string(text.x) + ", " + std::to_string(text.y) + ", " + std::to_string(text.z) + ", " + std::to_string(text.w) + ")", x, y, scale);
}

void DebugText::DrawAll()
{
	//���ׂĂ̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++)
	{
		//�X�v���C�g�`��
		sprites[i].DrawFG();
	}

	spriteIndex = 0;
}
#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

/// <summary>
/// �l�p�`�{�^��UI�N���X
/// </summary>
class UISquareButton
{
private:
	//�{�^���e�N�X�`��
	Sprite textureOn;
	Sprite textureOff;
	Sprite textureAdd;

	Vector2 pos;
	Vector2 size;

	Timer pushedEffectTimer;

public:

	void LoadTexture(const std::wstring& texturePath);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="texturePath">�摜�p�X �摜�͍�������off�A�E������on���̉摜���Ȃ������̂��g�p/param>
	/// <param name="pos">���W</param>
	void Initialize(const Vector2& pos);

	void Draw();

	/// <summary>
	/// �����ꂽ�Ƃ��̃G�t�F�N�g�J�n
	/// </summary>
	void StartPushedEffect();

	/// <summary>
	/// �}�E�X�J�[�\�����{�^���ɏd�Ȃ��Ă��邩��Ԃ�
	/// </summary>
	/// <returns></returns>
	bool IsOverlapMouseCursol();

	/// <summary>
	/// �{�^����������Ă��邩��Ԃ�
	/// </summary>
	/// <returns></returns>
	bool IsPushButton();

	/// <summary>
	/// �{�^�����������u�Ԃ��Ԃ�
	/// </summary>
	/// <returns></returns>
	bool IsTriggerButton();

	/// <summary>
	/// �{�^���������ꂽ��Ԃ��痣���ꂽ���ǂ����Ԃ�
	/// </summary>
	/// <returns></returns>
	bool IsReleaseButton();

	void SetPosition(const Vector2& pos);

	const Vector2& GetPosition() { return pos; }

	const Vector2& GetTexSize() { return size; }
};


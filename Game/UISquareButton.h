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
	DX12Library::Sprite textureOn;
	DX12Library::Sprite textureOff;
	DX12Library::Sprite textureAdd;

	DX12Library::Vector2 pos;
	DX12Library::Vector2 size;

	DX12Library::Timer pushedEffectTimer;

	bool isEnable = true;

	bool prevOverlap = false;

public:

	void LoadTexture(const std::string& texturePath);

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="texturePath">�摜�p�X �摜�͍�������off�A�E������on���̉摜���Ȃ������̂��g�p/param>
	/// <param name="pos">���W</param>
	void Initialize(const DX12Library::Vector2& pos);

	/// <summary>
	/// �`�� �e��X�V�����˂�
	/// </summary>
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

	/// <summary>
	/// �T�E���h�X�V
	/// </summary>
	void UpdateSound();

	void SetPosition(const DX12Library::Vector2& pos);

	void SetColor(const DX12Library::Vector4& color);

	/// <summary>
	/// �J�[�\���ƃ{�^�����d�Ȃ����Ƃ��A�e�N�X�`����ω������邩
	/// </summary>
	/// <param name="flag"></param>
	void SetIsEnable(bool flag) {
		if (isEnable) {
			UpdateSound();
		}
		isEnable = flag;
	}

	const DX12Library::Vector2& GetPosition() { return pos; }

	const DX12Library::Vector2& GetTexSize() { return size; }
};


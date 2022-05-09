#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

class Tutorial
{
private:
	//�t�@�C���f�B���N�g��
	static const std::string DIR;
	//�ʒu����
	static const float ADJUST_POS;

	//�e�N�X�`��
	DX12Library::Sprite popup;
	DX12Library::Sprite mouse;
	DX12Library::Sprite mouseCover;
	DX12Library::Sprite text;

	//����W
	DX12Library::Vector2 masterPos;

	//�^�C�}�[
	DX12Library::Timer moveMasterPosTimer;
	DX12Library::Timer roopEffectTimer;
	DX12Library::Timer moveOverlapMouseTimer;

	//�`���[�g���A���L����
	bool isEnable = false;
	//�ړ��̌������t�ɂ��邩
	bool reverseMove = false;
	//�ŏ��̃G�t�F�N�g�I���t���O
	bool isEndFirstEffect = false;

public:
	Tutorial();
	~Tutorial(){}

	void Initialize(bool isEnable);
	void Update();
	void Draw();

	void UpdateTexFromPhase();
	void UpdateTexPos();
	void UpdateTexPosOverlapMouse(DX12Library::Vector2& masterPos);

	//�}�E�X�ƃ`���[�g���A�����d�Ȃ��Ă��邩
	bool IsOverlapMouse();
};
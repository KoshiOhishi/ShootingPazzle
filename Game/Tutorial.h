#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

class Tutorial
{
private:
	//�t�@�C���f�B���N�g��
	static const std::wstring DIR;
	//�ʒu����
	static const float ADJUST_POS;

	//�e�N�X�`��
	Sprite popup;
	Sprite mouse;
	Sprite mouseCover;
	Sprite text;

	//����W
	Vector2 masterPos;

	//�^�C�}�[
	Timer moveMasterPosTimer;
	Timer roopEffectTimer;

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
};
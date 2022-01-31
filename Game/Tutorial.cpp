#include "Tutorial.h"
#include "Easing.h"
#include "DX12Util.h"
#include "DebugText.h"
#include "GameUtility.h"

const std::wstring Tutorial::DIR = L"Resources/Tutorial/";
const float Tutorial::ADJUST_POS = 10;

Tutorial::Tutorial()
{
	//�摜�ǂݍ���
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
	//�t���O�Z�b�g
	this->isEnable = isEnable;

	//����W�Z�b�g
	masterPos = { (float)DX12Util::GetWindowWidth(), (float)DX12Util::GetWindowHeight() - ADJUST_POS - popup.GetTexSize().y };

	//�e�N�X�`�������ʒu�Z�b�g
	popup.SetPosition(masterPos);
	mouse.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 124));
	mouseCover.SetPosition(mouse.GetPosition());
	text.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - text.GetTexSize().x / 4, 12));
	text.SetDrawRectangle(0,0,192,64);

	//�^�C�}�[������
	moveMasterPosTimer.SetTimer(0, 500);
	roopEffectTimer.SetTimer(0, 4000, true);
	roopEffectTimer.Start();

	reverseMove = false;
	isEndFirstEffect = false;
}

void Tutorial::Update()
{
	//�L���łȂ���΃��^�[��
	if (isEnable == false) {
		return;
	}

	UpdateTexFromPhase();

	UpdateTexPos();
}

void Tutorial::Draw()
{
	//�L���łȂ���΃��^�[��
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
	//���n�ړ����̓^�C�}�[�������l��
	if (moveMasterPosTimer.GetIsStart()) {
		roopEffectTimer.SetNowTime(0);
	}
	roopEffectTimer.Update();

	//��{�ʒu�X�V
	if (reverseMove) {
		masterPos.x = Easing::GetEaseValue(EASE_OUTQUINT, (double)DX12Util::GetWindowWidth() - ADJUST_POS - popup.GetTexSize().x, (double)DX12Util::GetWindowWidth(), moveMasterPosTimer);
	}
	else {
		masterPos.x = Easing::GetEaseValue(EASE_OUTQUINT, (double)DX12Util::GetWindowWidth(), (double)DX12Util::GetWindowWidth() - ADJUST_POS - popup.GetTexSize().x, moveMasterPosTimer);
	}

	int nowPhase = GameUtility::GetNowPhase();

	//�t�F�[�Y���ʂ̈ʒu�X�V
	popup.SetPosition(masterPos);
	text.SetPosition(masterPos + Vector2(popup.GetTexSize().x / 2 - text.GetTexSize().x / 4, 12));

	//�t�F�[�Y���Ƃ̈ʒu�X�V
	if (nowPhase == PHASE_GAME_FIRSTEFFECT ||
		nowPhase == PHASE_GAME_SETPOS) {
		//�}�E�X�̓���(���X�C���O)
		//���S�ʒu
		const Vector2 CENTER = masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 124);
		//���Z�ʒu
		float rad = (float)roopEffectTimer.GetNowTime() * 360 / 2000 * PI / 180;
		Vector2 addPos = { sin(rad) * 50, 0 };

		//�^�C�}�[��2000�����̎��͍��E�ɓ����A�����łȂ��Ƃ��͒�~
		if (roopEffectTimer.GetNowTime() >= 2000) {
			addPos = {};
		}
		//�K�p
		mouse.SetPosition(CENTER + addPos);
		mouseCover.SetPosition(mouse.GetPosition());

		//�e�L�X�g�`��͈̓Z�b�g
		text.SetDrawRectangle(0, 0, 192, 64);
	}
	else {
		//�}�E�X�̓���(�������𒆐S�ɃX�C���O)
		//�����S�ʒu
		const Vector2 CENTER = masterPos + Vector2(popup.GetTexSize().x / 2 - mouse.GetTexSize().x / 2, 250);
		//���Z�ʒu
		float rad = (float)roopEffectTimer.GetNowTime() * 360 / 2000 * PI / 180;
		Vector2 addPos = { sin(rad) * 30, -abs(cos(rad) * 20) - 130 };

		//�^�C�}�[��2000�����̎��͍��E�ɓ����A�����łȂ��Ƃ��͒�~
		if (roopEffectTimer.GetNowTime() >= 2000) {
			addPos = { 0,-20 - 130 };
		}

		//�K�p
		mouse.SetPosition(CENTER + addPos);
		mouseCover.SetPosition(mouse.GetPosition());

		//�e�L�X�g�`��͈̓Z�b�g
		text.SetDrawRectangle(192, 0, 192, 64);
	}

	//�w�莞�Ԃ͍��N���b�N�_��
	if (roopEffectTimer.GetNowTime() >= 2500 && roopEffectTimer.GetNowTime() < 3500) {
		float gb = Easing::GetEaseValue(EASE_INOUTSINE, 1, 0, roopEffectTimer, 2500, 3500);
		float a = Easing::GetEaseValue(EASE_INCUBIC, 1, 0, roopEffectTimer, 2500, 3500);
		mouseCover.SetColor({ 1,gb,gb, a });
	}
	else {
		mouseCover.SetColor({ 1, 1, 1, 0 });
	}
}

void Tutorial::UpdateTexFromPhase()
{
	static int prevPhase = PHASE_GAME_FIRSTEFFECT;
	int nowPhase = GameUtility::GetNowPhase();
	if (nowPhase != prevPhase) {

		//�ˏo����Ƃ��͋t�����Ɉړ�
		if (nowPhase == PHASE_GAME_AFTERSHOOT) {
			reverseMove = true;

			moveMasterPosTimer.Reset();
			moveMasterPosTimer.Start();
		}
		//FIRSTEFFECT�A�A���O���w�莞�A(�ŏ���FIRSTEFFECT����SETPOS�t�F�[�Y�̂Ƃ�)�ɂ͒ʏ�ړ�
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

	//�V�[����񂩂�X�v���C�g�e�N�X�`���X�V

}
//�ŏ���FIRSTEFFECT���Ă�₱��������ǂ��ɂ��������Ȃ�
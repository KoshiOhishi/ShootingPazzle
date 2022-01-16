#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Light.h"
#include "GameCamera.h"
#include "Object3D.h"
#include "TouchableObject.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Sound.h"
#include "RenderText.h"
#include "CollisionManager.h"
#include "UISquareButton.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"

class GamePlay :
	public Scene
{
private:
	//���C�g
	Light light;
	//�J����
	GameCamera camera;
	//�e
	MyBullet myBullet;

	//�X�e�[�W
	Stage stage;

	//�w�i
	Object3D objBG;
	ObjModel modelBG;

	//�O�i(��)
	Sprite sprWhite;

	//�O�i(��������)
	Sprite sprBlack;

	//�X�e�[�W�Z���N�g�ɖ߂邩�̃|�b�v�A�b�v
	Sprite sprPopUp;

	//�uClear�v���� (�G�t�F�N�g�p��2����)
	Sprite sprClearText[2];

	//UI(���Z�b�g)
	UISquareButton buttonReset;

	//UI(�X�e�[�W�Z���N�g�ɖ߂�)
	UISquareButton buttonBack;

	//UI(Yes)
	UISquareButton buttonYes;

	//UI(No)
	UISquareButton buttonNo;

	//�J���G�t�F�N�g�^�C�}�[
	Timer firstEffectTimer;

	//�|�b�v�A�b�v�o���^�C�}�[
	Timer dispPopUpTimer;

	//�V�[���`�F���W�^�C�}�[
	Timer sceneChangeTimer;

	//�N���A�G�t�F�N�g�^�C�}�[
	Timer clearEffectTimer;

	//1��ڂ̊J���G�t�F�N�g�I���t���O
	bool isEndFirstEffectOnce = false;

	//�|�b�v�A�b�v�\���t���O
	bool isDispPopup = false;


public:
	GamePlay();
	~GamePlay();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

	void UpdateDebugCamera();
	void UpdateTimer();
	void UpdateFirstEffect();
	void UpdateBG();
	void UpdateImgui();
	void UpdateUI();
	void UpdateStageBackPopUp();
	void UpdateClearEffect();

	void DrawWhiteEffect();
	void DrawBlackEffect();
	void DrawUI();
	void DrawStageBackPopUp();
	void DrawClearEffect();

	/// <summary>
	/// �Ֆʂ����Z�b�g���A�t�F�[�Y��SET_POS�ɂ��ǂ�
	/// </summary>
	void Reset();

	/// <summary>
	/// �N���A���Ă��邩�`�F�b�N �N���A���Ă�����t�F�[�Y��CLEAR��
	/// </summary>
	void CheckIsClear();
};


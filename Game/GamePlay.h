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
	bool isDispPopup = false;

	//UI(���Z�b�g)
	UISquareButton buttonReset;

	//UI(�X�e�[�W�Z���N�g�ɖ߂�)
	UISquareButton buttonBack;

	//UI(Yes)
	UISquareButton buttonYes;

	//UI(No)
	UISquareButton buttonNo;

	//�J���G�t�F�N�g�^�C�}�[
	Timer startEffectTimer;

	//�|�b�v�A�b�v�o���^�C�}�[
	Timer dispPopUpTimer;

	//�V�[���`�F���W�^�C�}�[
	Timer sceneChangeTimer;


public:
	GamePlay();
	~GamePlay();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

	void UpdateDebugCamera();
	void UpdateEffect();
	void UpdateBG();
	void UpdateImgui();
	void UpdateUI();
	void UpdatePopUp();

	void DrawEffect();
	void DrawUI();
	void DrawStageBackPopUp();

	/// <summary>
	/// �Ֆʂ����Z�b�g���A�t�F�[�Y��SET_POS�ɂ��ǂ�
	/// </summary>
	void Reset();

	/// <summary>
	/// �N���A���Ă��邩�`�F�b�N
	/// </summary>
	void CheckIsClear();
};


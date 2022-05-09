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
#include "Particle2D.h"
#include "Particle3D.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"
#include "Tutorial.h"

class GamePlay :
	public DX12Library::Scene
{
private:
	//���C�g
	DX12Library::Light light;
	//�J����
	GameCamera camera;
	//�e
	MyBullet myBullet;

	//�X�e�[�W
	Stage stage;

	//�w�i
	DX12Library::Object3D objBG;
	DX12Library::ObjModel modelBG;

	//UI�̃u���b�N
	DX12Library::Object3D objUISquareBlock;

	//�c��u���b�N�̐��� (3���܂őΉ�)
	DX12Library::Sprite sprRemainingBlockCount[3];

	//�O�i(��)
	DX12Library::Sprite sprWhite;

	//�O�i(��������)
	DX12Library::Sprite sprBlack;

	//�c��u���b�N����UI
	DX12Library::Sprite sprUIRemainingBlock;

	//�X�e�[�W�Z���N�g�ɖ߂邩�̃|�b�v�A�b�v
	DX12Library::Sprite sprPopUp;

	//�uClear�v���� (�G�t�F�N�g�p��2����)
	DX12Library::Sprite sprTextClear[2];

	//�uClearText�v����
	DX12Library::Sprite sprTextClearTime;

	//�N���A�^�C���̐��� (9999.999s�܂őΉ�)
	DX12Library::Sprite sprTextTimeNumber[9];

	//UI(���Z�b�g)
	UISquareButton buttonReset;

	//UI(�X�e�[�W�Z���N�g�ɖ߂�)
	UISquareButton buttonBack;

	//UI(Yes)
	UISquareButton buttonYes;

	//UI(No)
	UISquareButton buttonNo;

	//UI(OK)
	UISquareButton buttonOK;

	//�J���G�t�F�N�g�^�C�}�[
	DX12Library::Timer firstEffectTimer;

	//�|�b�v�A�b�v�o���^�C�}�[
	DX12Library::Timer dispPopUpTimer;

	//�V�[���`�F���W�^�C�}�[
	DX12Library::Timer sceneChangeTimer;

	//�N���A�G�t�F�N�g�^�C�}�[
	DX12Library::Timer clearEffectTimer;

	//�X�R�A�^�C�}�[
	DX12Library::Timer scoreTimer;

	//�p�[�e�B�N���ǉ��^�C�}�[
	DX12Library::Timer addParticleTimer;

	//1��ڂ̊J���G�t�F�N�g�I���t���O
	bool isEndFirstEffectOnce = false;

	//�|�b�v�A�b�v�\���t���O
	bool isDispPopup = false;

	//�p�[�e�B�N��
	DX12Library::Particle2D particle[2];

	//�N���A�G�t�F�N�g�̃p�[�e�B�N�����ǉ����ꂽ��
	bool addedParticleClearEffect = false;

	//�`���[�g���A���I�u�W�F�N�g
	Tutorial tutorial;

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
	void UpdateChangeScene();

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

	/// <summary>
	/// �c��u���b�N�J�E���g�̐����摜����
	/// </summary>
	/// <param name="pNumberTexArray"></param>
	/// <param name="arraySize"></param>
	/// <param name="drawNum"></param>
	void SetRemainingBlockCountTex(DX12Library::Sprite* pNumberTexArray, int arraySize, int drawNum, float numWidth, float numHeight);

	/// <summary>
	/// �c��u���b�N���̍��W�ꊇ�Z�b�g
	/// </summary>
	/// <param name="leftUpPos"></param>
	void SetRemainingBlockCountTexPos(DX12Library::Sprite* pNumberTexArray, int arraySizeconst, const DX12Library::Vector2& leftUpPos, float padding);

	/// <summary>
	/// �X�R�A�^�C���̃X�v���C�g����������
	/// </summary>
	/// <param name="pNumberTexArray"></param>
	/// <param name="arraySize"></param>
	/// <param name="startDrawTime"></param>
	void SetScoreTimeTex(DX12Library::Sprite* pNumberTexArray, int arraySize, int startDrawTime);

	/// <summary>
	/// �X�R�A�^�C����keta�Ŋۂ߂Ď擾
	/// </summary>
	/// <returns></returns>
	std::string GetStrScoreTime(int keta);
};


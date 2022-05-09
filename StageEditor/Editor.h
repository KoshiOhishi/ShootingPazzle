#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Light.h"
#include "Camera.h"
#include "Object3D.h"
#include "TouchableObject.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Sound.h"
#include "RenderText.h"
#include "CollisionManager.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "EditorStage.h"
#include "StartLane.h"

#include "Command.h"

//���[�h
enum EditorMode {
	MODE_ADD,
	MODE_DELETE,
	MODE_OPTION
};

//�I�v�V�������[�h
enum OptionMode {
	OPTION_CHANGE_STAGESIZE,
	OPTION_SET_STARTLANE,
	OPTION_SAVE,
	OPTION_LOAD
};

class Editor :
	public DX12Library::Scene
{
private:
	//���C�g
	DX12Library::Light light;
	//�J����
	DX12Library::Camera camera;
	//�^�C�}�[
	DX12Library::Timer timer;
	//�X�e�[�W
	EditorStage stage;

	//�w�i
	DX12Library::Object3D objBG;
	DX12Library::ObjModel modelBG;

	//Editor
	//���[�h
	int mode = MODE_ADD;
	int objectType = OBJECTTYPE_BLOCK;
	int blockType = BLOCKTYPE_SQUARE;
	int floorType = FLOORTYPE_NORMAL;
	//����܂ł̏Փˉ񐔁@0�ŉ��Ȃ��u���b�N
	int breakupCount = 0;
	int blockColor = BLOCK_COLOR_NONE;
	//���݂̃J�[�\���ʒu
	StageVec2 nowCursolPos;
	//�I�v�V�����ł̃��[�h
	int optionMode = OPTION_CHANGE_STAGESIZE;

	//�\���p�I�u�W�F�N�g
	DX12Library::Object3D objDispBlock;
	DX12Library::Object3D objDispFloor;
	DX12Library::Object3D objDispFloorSub;	//�⏕�I�u�W�F�N�g

	//�X�^�[�g���[���\���p
	StartLane startLane[2];

	//SaveLoad�p���O�i�[
	std::string ioname;

	//�������͒���
	bool isEnteringIOName = false;

	//Stage�č\�z�p�O�t���[���X�e�[�W�T�C�Y
	StageVec2 prevSize;

	//ImGui�X���C�_�[WidthDepth
	int sliderWidth;
	int sliderDepth;

	//�J�[�\���ʒu�\��
	DX12Library::RenderText txtCursol;

public:
	Editor();
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void UpdateCamera();
	void UpdateEdit();

	void UpdateAdd();
	void UpdateDelete();
	void UpdateOption();
	void UpdateObject();
	void UpdateStage();
	void UpdateDispObject();
	void UpdateStartLane();

	void DrawStartLane();
	void DrawDispBlock();
	void DrawDispFloor();

	void Save();

	void Load();

	void UpdateImgui();

	/// <summary>
	/// ���݂̃X�e�[�W��̃J�[�\���̈ʒu���擾
	/// </summary>
	void CalcNowCursolPos();

	/// <summary>
	/// �X�e�[�W���č\�z����
	/// </summary>
	void ReCreateStage(const StageVec2& prevSize, const StageVec2& nowSize);

	bool IsInsideCursol();
};
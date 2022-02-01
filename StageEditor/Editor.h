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

enum EditorMode {
	MODE_ADD,
	MODE_DELETE,
	MODE_OPTION
};

enum EditObjectType {
	OBJECTTYPE_BLOCK,
	OBJECTTYPE_FLOOR
};

enum OptionMode {
	OPTION_SET_STARTLANE
};

class Editor :
	public Scene
{
private:
	//���C�g
	Light light;
	//�J����
	Camera camera;
	//�^�C�}�[
	Timer timer;
	//�X�e�[�W
	EditorStage stage;


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
	int optionMode = OPTION_SET_STARTLANE;

	//�\���p�I�u�W�F�N�g
	Object3D objDispBlock;
	Object3D objDispFloor;
	Object3D objDispFloorSub;	//�⏕�I�u�W�F�N�g
	StartLane startLane[2];

	//SaveLoad�p���O�i�[
	std::string ioname;

	//�������͒���
	bool isEnteringIOName = false;

	//ImGui�X���C�_�[WidthDepth
	int sliderWidth;
	int sliderDepth;

public:
	Editor(){}
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void UpdateEdit();

	void UpdateAdd();
	void UpdateDelete();
	void UpdateObject();
	void UpdateDispObject();
	void UpdateStartLane();

	void DrawStartLane();
	void DrawBlock();
	void DrawFloor();

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
	void ReCreateStage(unsigned short width, unsigned short depth);
};
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Vector4.h"

//�~����
const float PI = 3.14159265359f;
//1�}�X�̒���
const float ONE_CELL_LENGTH = 5.0f;
//���f���̃f�B���N�g��
const std::string MODEL_DIR = "Resources/Model/";
//2D�e�N�X�`���̃f�B���N�g��
const std::wstring TEX_DIR_UTIL = L"Resources/2DTexture/Utility/";
const std::wstring TEX_DIR_TITLE = L"Resources/2DTexture/Title/";
const std::wstring TEX_DIR_STAGESELECT = L"Resources/2DTexture/StageSelect/";
const std::wstring TEX_DIR_GAMEPLAY = L"Resources/2DTexture/GamePlay/";
const std::string TEX_DIR_GAMEPLAY_S = "Resources/2DTexture/GamePlay/";

const std::string STAGE_DIR = "Resources/StageData/";

/// <summary>
/// �X�e�[�W���W ���オ���_
/// </summary>
struct StageVec2 {
	unsigned short x = 0;
	unsigned short y = 0;
};

/// <summary>
/// �u���b�N�^�C�v
/// </summary>
enum BlockType {
	BLOCKTYPE_SQUARE,
	BLOCKTYPE_TRIANGLE_NO_LEFTTOP,		//����̊p���Ȃ��O�p�`
	BLOCKTYPE_TRIANGLE_NO_RIGHTTOP,		//�E��̊p���Ȃ��O�p�`
	BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM,	//�����̊p���Ȃ��O�p�`
	BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM,	//�E���̊p���Ȃ��O�p�`
};

enum FloorType {
	FLOORTYPE_NORMAL,
	FLOORTYPE_TURN_LEFT,
	FLOORTYPE_TURN_RIGHT,
	FLOORTYPE_TURN_UP,
	FLOORTYPE_TURN_DOWN,
	FLOORTYPE_BREAK,
	FLOORTYPE_SWITCH_NONE,
	FLOORTYPE_SWITCH_RED,
	FLOORTYPE_SWITCH_BLUE,
	FLOORTYPE_SWITCH_YELLOW,
	FLOORTYPE_SWITCH_GREEN,
};

//�Q�[���t�F�[�Y
enum Phase
{
	PHASE_STAGESELECT_SELECT,
	PHASE_GAME_FIRSTEFFECT,
	PHASE_GAME_SETPOS,
	PHASE_GAME_SETANGLE,
	PHASE_GAME_AFTERSHOOT,
	PHASE_GAME_CLEAR
};

//�X�e�[�W�̐F
enum StageColor
{
	STAGE_COLOR_NONE,
	STAGE_COLOR_RED,
	STAGE_COLOR_BLUE,
	STAGE_COLOR_YELLOW,
	STAGE_COLOR_GREEN,
};

struct StageHeader
{
	unsigned short width = 20;
	unsigned short depth = 20;
	unsigned short startLineZ = depth - 12;
	unsigned short blockCount = 0;
	unsigned short floorCount = 0;
};

struct StageBlock
{
	unsigned short stagePosX = 0;
	unsigned short stagePosY = 0;
	char type = 0;
	unsigned short breakupCount = 0;	//�u���b�N������̏Փ˂ŉ��邩 (�ǂݍ��ݎ�0���Ɖ��Ȃ��u���b�N�ɂȂ�)
	char blockColor = 0;
};

struct StageFloor
{
	unsigned short stagePosX = 0;
	unsigned short stagePosY = 0;
	char type = 0;
};

class GameUtility
{
	//�萔
public:
	static const Vector4 COLOR_VALUE[5];
	static const Vector4 COLOR_VALUE_BREAKABLE[5];

private:
	//���݂̃t�F�[�Y
	static int nowPhase;
	//�X�e�[�W�T�C�Y���i�[����|�C���^
	static StageVec2* pStageSize;
	//�X�e�[�W�̌��݂̐F
	static int stageColor;
	//���݂̃X�e�[�W�p�X
	static std::string nowStagePath;
	//�Q�[�����ꎞ��~��Ԃ�
	static bool isPause;

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// ���[���h���W��xz���ʂ���X�e�[�W��̍��W�ɕϊ�
	/// </summary>
	/// <param name="worldX">���[���h���Wx</param>
	/// <param name="worldZ">���[���h���Wz</param>
	/// <returns></returns>
	static const StageVec2& CalcWorldPos2StagePos(float worldX, float worldZ);

	/// <summary>
	/// �X�e�[�W��̍��W���烏�[���h���W��xz���ʂɕϊ�
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <param name="dstWorldX">���[���h���Wx</param>
	/// <param name="dstWorldZ">���[���h���Wz</param>
	static void CalcStagePos2WorldPos(const StageVec2& stagePos, float* dstWorldX = nullptr, float* dstWorldZ = nullptr);

	/// <summary>
	/// ���݂̃Q�[���t�F�[�Y���Z�b�g
	/// </summary>
	/// <param name="phase">PHASE_...</param>
	static void SetNowPhase(int phase) { nowPhase = phase; }

	/// <summary>
	/// ���݃v���C���̃X�e�[�W�T�C�Y�ϐ��̃|�C���^���Z�b�g����
	/// </summary>
	/// <param name="pStageSize"></param>
	static void SetPStageSize(StageVec2* pStageSize) { GameUtility::pStageSize = pStageSize; }

	/// <summary>
	/// �X�e�[�W�̐ݒ�F���Z�b�g���� �����F�̃u���b�N���ʂ蔲������悤�ɂȂ�
	/// </summary>
	/// <param name="stageColor"></param>
	static void SetStageColor(int stageColor) { GameUtility::stageColor = stageColor; }

	/// <summary>
	/// ���݃v���C���̃X�e�[�W�̃p�X���Z�b�g
	/// </summary>
	/// <param name="path"></param>
	static void SetNowStagePath(const std::string& path) { GameUtility::nowStagePath = path; }

	/// <summary>
	/// �|�[�Y�����̃t���O�Z�b�g
	/// </summary>
	/// <param name="isPause"></param>
	static void SetIsPause(bool isPause) { GameUtility::isPause = isPause; }


	/// <summary>
	/// ���݂̃t�F�[�Y�擾
	/// </summary>
	/// <returns></returns>
	static const int GetNowPhase() { return nowPhase; }

	/// <summary>
	/// �X�e�[�W�̐ݒ�F�擾
	/// </summary>
	/// <returns></returns>
	static const int GetStageColor() { return stageColor; }

	/// <summary>
	/// ���݃v���C���̃X�e�[�W�̃p�X���擾
	/// </summary>
	/// <returns></returns>
	static const std::string& GetNowStagePath() { return nowStagePath; }

	/// <summary>
	/// �|�[�Y�����ǂ����擾
	/// </summary>
	/// <returns></returns>
	static bool GetIsPause() { return isPause; }
};
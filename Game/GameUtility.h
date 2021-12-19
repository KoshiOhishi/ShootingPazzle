#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#define PI 3.14159265359f
//1�}�X�̒���
#define ONE_CELL_LENGTH (float)5.0f

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
	FLOORTYPE_BREAK
};

//�Q�[���t�F�[�Y
enum Phase
{
	PHASE_FIRSTEFFECT,
	PHASE_SETPOS,
	PHASE_SETANGLE,
	PHASE_AFTERSHOOT
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
};

struct StageFloor
{
	unsigned short stagePosX = 0;
	unsigned short stagePosY = 0;
	char type = 0;
};

class GameUtility
{

private:
	//���݂̃t�F�[�Y
	static int nowPhase;
	//�X�e�[�W�T�C�Y���i�[����|�C���^
	static StageVec2* pStageSize;

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	static void SetNowPhase(int phase) { nowPhase = phase; }
	static void SetPStageSize(StageVec2* pStageSize) { GameUtility::pStageSize = pStageSize; }

	static int GetNowPhase() { return nowPhase; }

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
};
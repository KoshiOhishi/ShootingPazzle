#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Block.h"
#include "Stage.h"

#define PI 3.14159265359f
//1�}�X�̒���
#define ONE_CELL_LENGTH (float)5.0f

//�Q�[���t�F�[�Y
enum Phase
{
	PHASE_SETPOS,
	PHASE_SETANGLE,
	PHASE_AFTERSHOOT
};

struct StageHeader
{
	unsigned short width = 20;
	unsigned short depth = 20;
	unsigned short startLineZ = depth - 12;
	unsigned short objectCount = 0;
};

struct StageObject
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

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	static void SetNowPhase(int phase) { nowPhase = phase; }
	static int GetNowPhase() { return nowPhase; }

	/// <summary>
	/// ���[���h���W��xz���ʂ���X�e�[�W��̍��W�ɕϊ�
	/// </summary>
	/// <param name="worldX">���[���h���Wx</param>
	/// <param name="worldZ">���[���h���Wz</param>
	/// <returns></returns>
	static const StageVec2& CalcWorldPos2StagePos(float worldX, float worldZ, const StageVec2& stageSize);

	/// <summary>
	/// �X�e�[�W��̍��W���烏�[���h���W��xz���ʂɕϊ�
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <param name="dstWorldX">���[���h���Wx</param>
	/// <param name="dstWorldZ">���[���h���Wz</param>
	static void CalcStagePos2WorldPos(const StageVec2& stagePos, const StageVec2& stageSize, float* dstWorldX = nullptr, float* dstWorldZ = nullptr);
};
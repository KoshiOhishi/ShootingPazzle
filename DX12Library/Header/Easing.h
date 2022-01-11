#pragma once
#include <string>
#include "Timer.h"

//�Q�l:https://easings.net/ja

class Easing
{
#define PI 3.14159265359

#define EASE_LINE -1
#define EASE_INSIDE 0
#define EASE_OUTSINE 1
#define EASE_INOUTSINE 2
#define EASE_INQUAD 3
#define EASE_OUTQUAD 4
#define EASE_INOUTQUAD 5
#define EASE_INCUBIC 6
#define EASE_OUTCUBIC 7
#define EASE_INOUTCUBIC 8
#define EASE_INQUART 9
#define EASE_OUTQUART 10
#define EASE_INOUTQUART 11
#define EASE_INQUINT 12
#define EASE_OUTQUINT 13
#define EASE_INOUTQUINT 14
#define EASE_INEXPO 15
#define EASE_OUTEXPO 16
#define EASE_INOUTEXPO 17
#define EASE_INCIRC 18
#define EASE_OUTCIRC 19
#define EASE_INOUTCIRC 20
#define EASE_INBACK 21
#define EASE_OUTBACK 22
#define EASE_INOUTBACK 23
#define EASE_INELASTIC 24
#define EASE_OUTELASTIC 25
#define EASE_INOUTELASTIC 26
#define EASE_INBOUNCE 27
#define EASE_OUTBOUNCE 28
#define EASE_INOUTBOUNCE 29

private:
	static const double c1;
	static const double c2;
	static const double c3;
	static const double c4;
	static const double c5;
	static const double n1;
	static const double d1;

	/// <summary>
	/// �y�ʉ�pow ���R����̂Ƃ��͂��������g������
	/// </summary>
	/// <param name="x">�l</param>
	/// <param name="t">�ݏ搔</param>
	/// <returns>����</returns>
	static double Pow(double x, int t);

public:
	/// <summary>
	/// �C�[�W���O���������l���擾
	/// </summary>
	/// <param name="type">�C�[�W���O�̎��</param>
	/// <param name="start">t=0�̎��̒l</param>
	/// <param name="end">t=1�̎��̒l</param>
	/// <param name="timer">�^�C�}�[ ���J�n-�I����0���傫�����邱��</param>
	/// <param name="startTime">t=0�̎��̎��� ������O��0�Œ�</param>
	/// <param name="endTime">t=1�̎��̎��ԁ@����ȍ~��1�Œ�</param>
	/// <returns>�C�[�W���O���������l</returns>
	static double GetEaseValue(int type, double start, double end, const Timer& timer, double startTime = -1, double endTime = -1);

	/// <summary>
	/// �C�[�W���O���������l���擾
	/// </summary>
	/// <param name="type">�C�[�W���O�̎��</param>
	/// <param name="start">t=0�̎��̒l</param>
	/// <param name="end">t=1�̎��̒l</param>
	/// <param name="t">0�`1�̊Ԃ̐��l</param>
	/// <returns>�C�[�W���O���������l</returns>
	static double GetEaseValue(int type, double start, double end, double t);

	static const std::string& GetFuncName(int type);
};


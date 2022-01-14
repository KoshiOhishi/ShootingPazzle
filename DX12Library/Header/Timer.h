#pragma once

/// <summary>
/// �~���b���x�^�C�}�[
/// </summary>
class Timer
{
private:
	int nowTime;
	int startTime;
	int endTime;
	int countStartTime;
	int prevTime;
	float updateSpeed = 1.0f;
	bool isStart = false;
	bool isLoop = false;

private:
	static int GetClockNowTime();

public:
	Timer();
	Timer(int start, int end, bool isLoop = false, float speed = 1.0f);

	/// <summary>
	/// �^�C�}�[���Z�b�g
	/// </summary>
	/// <param name="start">�J�n�l</param>
	/// <param name="end">�I���l</param>
	/// <param name="speed">�X�V�{��</param>
	void SetTimer(int start, int end, bool isLoop = false, float speed = 1.0f);

	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �^�C�}�[�̍X�V���J�n
	/// </summary>
	void Start();

	/// <summary>
	/// �^�C�}�[�̍X�V���ꎞ��~
	/// </summary>
	void Stop();

	/// <summary>
	/// �^�C�}�[�̌��ݒl���J�n�l�ɖ߂��A�X�V���ꎞ��~
	/// </summary>
	void Reset();

	/// <summary>
	/// �^�C�}�[�̌��ݎ��Ԃ��Z�b�g �J�n�l <= time <= �I���l�ɃN�����v�����B
	/// </summary>
	/// <param name="time"></param>
	void SetNowTime(int time);

	/// <summary>
	/// �^�C�}�[���J�n���Ă��邩���擾
	/// </summary>
	/// <returns></returns>
	bool GetIsStart()const { return isStart; }

	/// <summary>
	/// �^�C�}�[���I���l�ɒB���Ă��邩���擾
	/// </summary>
	/// <returns></returns>
	bool GetIsEnd()const { return nowTime >= endTime; }

	/// <summary>
	/// �J�n�l���擾
	/// </summary>
	/// <returns></returns>
	int GetStartTime()const { return startTime; }

	/// <summary>
	/// �I���l���擾
	/// </summary>
	/// <returns></returns>
	int GetEndTime()const { return endTime; }

	/// <summary>
	/// ���ݒl���擾
	/// </summary>
	/// <returns></returns>
	int GetNowTime()const { return nowTime; }
};


#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class Camera
{
protected: //�G�C���A�X
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
	using string = std::string;

public: //�T�u�N���X
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	static const float CAMERA_NEAR;
	static const float CAMERA_FAR;
	static const float CAMERA_VIEWING_ANGLE;

protected: //�����o�ϐ�

// �ˉe�ϊ��s��
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();

	// �r���[�ϊ��s��
	XMMATRIX matView = DirectX::XMMatrixIdentity();

	// ��]�s��
	XMMATRIX matRotation = DirectX::XMMatrixIdentity();

	//�r���{�[�h�s��
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();

	//Y�����r���{�[�h�s��
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();

	// ���_���W
	Vector3 eye = { 0,0,-100 };

	// �����_���W
	Vector3 target = { 0, 0, 0 };

	// ������x�N�g��
	Vector3 up = { 0, 1, 0 };

	// �J������]�p
	Vector3 rotation = { 0,0,0 };

	//�_�[�e�B�t���O
	bool dirty = false;

public: //�����o�֐�

	//������
	virtual void Initialize();

	//�X�V
	virtual void Update();

	/// <summary>
	/// �J�������ړ������鏈��
	/// </summary>
	/// <param name="vector">�ړ��ʃx�N�g��</param>
	/// <param name="isAutoDirAxisX">�J�����������Ă����������Ɉړ����邩�ǂ����iX����j</param>
	/// <param name="isAutoDirAxisY">�J�����������Ă����������Ɉړ����邩�ǂ����iY����j</param>
	/// <param name="isAutoDirAxisZ">�J�����������Ă����������Ɉړ����邩�ǂ����iZ����j</param>
	void MoveCamera(const Vector3& vector, bool isAutoDirAxisX = false, bool isAutoDirAxisY = true, bool isAutoDirAxisZ = false);

	/// <summary>
	/// �J�������ړ������鏈��
	/// </summary>
	/// <param name="x">�ړ���x</param>
	/// <param name="y">�ړ���y</param>
	/// <param name="z">�ړ���z</param>
	/// <param name="isAutoDirAxisX">�J�����������Ă����������Ɉړ����邩�ǂ����iX����j</param>
	/// <param name="isAutoDirAxisY">�J�����������Ă����������Ɉړ����邩�ǂ����iY����j</param>
	/// <param name="isAutoDirAxisZ">�J�����������Ă����������Ɉړ����邩�ǂ����iZ����j</param>
	void MoveCamera(const float x, const float y, const float z, bool isAutoDirAxisX = false, bool isAutoDirAxisY = true, bool isAutoDirAxisZ = false);

	/// <summary>
	/// �J���������̊p�x����w��ʉ�]
	/// </summary>
	/// <param name="axis">��]��</param>
	/// <param name="digrees">��]��</param>
	void RotateCamera(const Vector3& axis, float digrees);

	/// <summary>
	/// �r���[�s��̍X�V����
	/// </summary>
	void UpdateViewMatrix();

	//Getter
public:
	//�r���[�v���W�F�N�V�����s��擾
	const XMMATRIX GetViewProjection()const;

	//�r���{�[�h�s��擾
	const XMMATRIX& GetBillboard()const { return matBillboard; }

	//Y�����r���{�[�h�s��擾
	const XMMATRIX& GetBillboardY()const { return matBillboardY; }

	/// <summary>
	/// ���_���W�擾
	/// </summary>
	/// <returns>���_���W</returns>
	const Vector3& GetPosition()const { return eye; }

	/// <summary>
	/// �����_���W�擾
	/// </summary>
	/// <returns>�����_���W</returns>
	const Vector3 GetTarget()const { return eye + GetCameraDir() * GetDistance(); }

	/// <summary>
	/// ������x�N�g���擾
	/// </summary>
	/// <returns>������x�N�g��</returns>
	const Vector3 GetUpVec()const;

	/// <summary>
	/// ���_���W���璍���_���W�̋������擾
	/// </summary>
	/// <returns>���_���W���璍���_���W�̋���</returns>
	const float GetDistance()const;

	/// <summary>
	/// �J�����̉�]�p���擾
	/// </summary>
	/// <returns>��]�p(x = pitch, y = yaw, z = roll)</returns>
	const Vector3& GetRotation()const { return rotation; }

	/// <summary>
	/// �J�����̉�]�s����擾
	/// </summary>
	/// <returns>��]�s��</returns>
	const XMMATRIX& GetRotMatrix()const { return matRotation; }

	/// <summary>
	/// �r���[�s����擾
	/// </summary>
	/// <returns>�r���[�s���</returns>
	const XMMATRIX GetViewMatrix()const { return matView; }

	/// <summary>
	/// �ˉe�ϊ��s����擾
	/// </summary>
	/// <returns>�ˉe�ϊ��s��</returns>
	const XMMATRIX& GetProjectionMatrix()const { return matProjection; }

	/// <summary>
	/// �J�����̌����Ă���������擾
	/// </summary>
	/// <returns></returns>
	const Vector3 GetCameraDir()const;


//Setter
public:
	//���_���W���Z�b�g
	void SetEye(const Vector3& eye);
	void SetEye(const float x, const float y, const float z);
	//�����_���W���Z�b�g
	void SetTarget(const Vector3& target);
	void SetTarget(const float x, const float y, const float z);
	//������x�N�g�����Z�b�g
	void SetUpVec(const Vector3& up);
	void SetUpVec(const float x, const float y, const float z);

	/// <summary>
	/// ���_���W�A�����_���W�A������x�N�g�����Z�b�g (�������p)
	/// </summary>
	/// <param name="eye">���_���W</param>
	/// <param name="target">�����_���W</param>
	/// <param name="up">��x�N�g��</param>
	void SetEyeTargetUpVec(const Vector3& eye, const Vector3& target, const Vector3& up);

	/// <summary>
	/// ���_���W�A�����_���W�A������x�N�g�����Z�b�g (�������p)
	/// </summary>
	/// <param name="eyeX">���_X���W</param>
	/// <param name="eyeY">���_Y���W</param>
	/// <param name="eyeZ">���_Z���W</param>
	/// <param name="targetX">�����_X���W</param>
	/// <param name="targetY">�����_Y���W</param>
	/// <param name="targetZ">�����_Z���W</param>
	/// <param name="upX">��x�N�g��X</param>
	/// <param name="upY">��x�N�g��Y</param>
	/// <param name="upZ">��x�N�g��Z</param>
	void SetEyeTargetUpVec(
		const float eyeX, const float eyeY, const float eyeZ,
		const float targetX, const float targetY, const float targetZ,
		const float upX, const float upY, const float upZ);

	/// <summary>
	/// ���_���W�A�����_���W�܂ł̋������܂Ƃ߂ăZ�b�g
	/// </summary>
	/// <param name="pos">���_���W</param>
	/// <param name="distance">���_���W���璍���_���W�̋���</param>
	void SetPositionAndDistance(const Vector3& pos, const float distance);

	/// <summary>
	/// ���_���W�A�����_���W�܂ł̋������܂Ƃ߂ăZ�b�g
	/// </summary>
	/// <param name="posX">���_X���W</param>
	/// <param name="posY">���_Y���W</param>
	/// <param name="posZ">���_Z���W</param>
	/// <param name="distance">���_���W���璍���_���W�̋���</param>
	void SetPositionAndDistance(const float posX, const float posY, const float posZ, const float distance);

	/// <summary>
	/// ���_���W���Z�b�g
	/// </summary>
	/// <param name="pos">���_���W</param>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// ���_���W���Z�b�g
	/// </summary>
	/// <param name="x">���_X���W</param>
	/// <param name="y">���_Y���W</param>
	/// <param name="z">���_Z���W</param>
	void SetPosition(const float x, const float y, const float z);

	/// <summary>
	/// ���_���W���璍���_���W�̋������Z�b�g
	/// </summary>
	/// <param name="distance">���_���W���璍���_���W�̋���</param>
	void SetDistance(const float distance);

	/// <summary>
	/// �J�����̉�]�p���Z�b�g
	/// </summary>
	/// <param name="vector">��]��(�x)</param>
	void SetRotation(const Vector3& rotation);

	/// <summary>
	/// �J�����̉�]�p���Z�b�g
	/// </summary>
	/// <param name="pitch">X����]�p(�x)</param>
	/// <param name="yaw">Y����]�p(�x)</param>
	/// <param name="roll">Z����]�p(�x)</param>
	void SetRotation(const float pitch, const float yaw, const float roll);

private:
	/// <summary>
	/// ���_���W���w�蕪���Z����
	/// </summary>
	/// <param name="eyeX">X���W</param>
	/// <param name="eyeY">Y���W</param>
	/// <param name="eyeZ">Z���W</param>
	void AddEye(const float eyeX, const float eyeY, const float eyeZ);

	/// <summary>
	/// �����_���W���w�蕪���Z����
	/// </summary>
	/// <param name="targetX">X���W</param>
	/// <param name="targetY">Y���W</param>
	/// <param name="targetZ">Z���W</param>
	void AddTarget(const float targetX, const float targetY, const float targetZ);

	/// <summary>
	/// ������x�N�g�����w�蕪���Z����
	/// </summary>
	/// <param name="upX">x����</param>
	/// <param name="upY">y����</param>
	/// <param name="upZ">z����</param>
	void AddUp(const float upX, const float upY, const float upZ);
};


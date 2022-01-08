#include "Camera.h"
#include "DX12Util.h"
#include "Quaternion.h"

using namespace DirectX;

const float Camera::CAMERA_NEAR = 0.1f;
const float Camera::CAMERA_FAR = 1000.0f;
const float Camera::CAMERA_VIEWING_ANGLE = 60.0f;

void Camera::Initialize()
{
	//�����l���
	matProjection = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixIdentity();
	matRotation = DirectX::XMMatrixIdentity();
	matBillboard = DirectX::XMMatrixIdentity();
	matBillboardY = DirectX::XMMatrixIdentity();

	eye = { 0,0,-100 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	//�ˉe�s��̍쐬
	//�������e
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(CAMERA_VIEWING_ANGLE),
		(float)DX12Util::GetWindowWidth() / DX12Util::GetWindowHeight(),
		CAMERA_NEAR,
		CAMERA_FAR
	);

	UpdateViewMatrix();
}

void Camera::Update()
{
	//�l�̕ύX���������������s��X�V
	if (dirty)
	{
		UpdateViewMatrix();
		dirty = false;
	}
}

void Camera::AddEye(const float eyeX, const float eyeY, const float eyeZ)
{
	this->eye.x += eyeX;
	this->eye.y += eyeY;
	this->eye.z += eyeZ;
	dirty = true;
}

void Camera::AddTarget(const float targetX, const float targetY, const float targetZ)
{
	this->target.x += targetX;
	this->target.y += targetY;
	this->target.z += targetZ;
	dirty = true;
}

void Camera::AddUp(const float upX, const float upY, const float upZ)
{
	this->up.x += upX;
	this->up.y += upY;
	this->up.z += upZ;
	dirty = true;
}

void Camera::MoveCamera(const Vector3& vector, bool isAutoDirAxisX, bool isAutoDirAxisY, bool isAutoDirAxisZ)
{
	//�J�����̉�]�ʂ����W�A���ɂ���
	Vector3 rad = {
		(rotation.x) * 3.14159265359f / 180,
		(rotation.y) * 3.14159265359f / 180,
		(rotation.z) * 3.14159265359f / 180 };

	Quaternion qx, qy, qz, qq;

	//�ړ������x�N�g���̉�]
	//X�����]
	if (isAutoDirAxisX) {
		qx = Quaternion::CreateQuaternion({ 1,0,0 }, rad.x);
	}
	else {
		qx = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ vector.x, vector.y, vector.z }, qx);

	//Y�����]
	if (isAutoDirAxisY) {
		qy = Quaternion::CreateQuaternion({ 0,1,0 }, rad.y);
	}
	else {
		qy = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ qq.x, qq.y, qq.z }, qy);

	//Z�����]
	if (isAutoDirAxisZ) {
		qz = Quaternion::CreateQuaternion({ 0,0,1 }, rad.z);
	}
	else {
		qz = Quaternion::CreateQuaternion(0, 0, 0, 1);
	}
	qq = Quaternion::CreateQuaternion({ qq.x, qq.y, qq.z }, qz);

	//Vector3�ɕϊ�
	Vector3 dir = { qq.x, qq.y, qq.z };

	//�ړ�
	AddEye(dir.x, dir.y, dir.z);
	AddTarget(dir.x, dir.y, dir.z);
}

void Camera::MoveCamera(const float x, const float y, const float z, bool isAutoDirAxisX, bool isAutoDirAxisY, bool isAutoDirAxisZ)
{
	Vector3 vector = { x,y,z };
	MoveCamera(vector, isAutoDirAxisX, isAutoDirAxisY, isAutoDirAxisZ);
}

void Camera::RotateCamera(const Vector3& axis, const float digrees)
{
	//���W�A���ɕϊ�
	float rad = digrees * 3.14159265359f / 180;

	//��]�p�N�H�[�^�j�I������
	Quaternion q(Vector3(axis.x, axis.y, axis.z), rad);

	//��]�s��𐶐�
	XMMATRIX matRot = Quaternion::Rotate(q);

	matRotation *= matRot;

	dirty = true;
}

void Camera::UpdateViewMatrix()
{
	//���_���W
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	//�����_���W
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	//(����)�����
	XMVECTOR upVector = XMLoadFloat3(&up);

	matRotation = XMMatrixIdentity();

	RotateCamera({ 0,0,1 }, rotation.z);
	RotateCamera({ 1,0,0 }, rotation.x);
	RotateCamera({ 0,1,0 }, rotation.y);

	//�]�u�ɂ��t�s��(�t��])���v�Z
	matView = XMMatrixTranspose(matRotation);


	//���s�ړ��̋t�����߂�
	//���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	XMVECTOR tX = XMVector3Dot(matRotation.r[0], reverseEyePosition);	//X����
	XMVECTOR tY = XMVector3Dot(matRotation.r[1], reverseEyePosition);	//Y����
	XMVECTOR tZ = XMVector3Dot(matRotation.r[2], reverseEyePosition);	//Z����
	//��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//�r���[�s��ɕ��s�ړ�������ݒ�
	matView.r[3] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	//�r���{�[�h�s��
	matBillboard.r[0] = matRotation.r[0];
	matBillboard.r[1] = matRotation.r[1];
	matBillboard.r[2] = matRotation.r[2];
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

#pragma region Y�����r���{�[�h�s��̌v�Z
	//�J����X���AY���AZ��
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X���͋���
	ybillCameraAxisX = matRotation.r[0];
	//Y���̓��[���h���W�n��Y��
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z����X����Y���̊O�ςŋ��܂�
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	//Y�����r���{�[�h�s��
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma endregion
}

const XMMATRIX& Camera::GetViewProjection()const
{
	XMMATRIX tmp = matView * matProjection;
	return tmp;
}

const Vector3 Camera::GetUpVec() const
{
	Vector3 tmp = { 0,1,0 };

	Quaternion q1(matRotation);
	Quaternion q2(tmp, q1);

	return Vector3{ q2.x,q2.y,q2.z };
}

const float Camera::GetDistance()const
{
	XMVECTOR sub = XMLoadFloat3(&target) - XMLoadFloat3(&eye);
	XMVECTOR t = XMVector3Length(sub);
	return t.m128_f32[0];
}

const Vector3 Camera::GetCameraDir()const
{
	Vector3 tmp = { 0,0,1 };
	
	Quaternion q1(matRotation);
	Quaternion q2(tmp, q1);

	return Vector3{ q2.x,q2.y,q2.z };
}

void Camera::SetEye(const Vector3& eye)
{
	this->eye = eye;
	dirty = true;
}
void Camera::SetEye(const float x, const float y, const float z)
{
	this->eye.x = x;
	this->eye.y = y;
	this->eye.z = z;
	dirty = true;
}
//�����_���W���Z�b�g
void Camera::SetTarget(const Vector3& target)
{
	this->target = target;
	dirty = true;
}
void Camera::SetTarget(const float x, const float y, const float z)
{
	this->target.x = x;
	this->target.y = y;
	this->target.z = z;
	dirty = true;
}
//������x�N�g�����Z�b�g
void Camera::SetUpVec(const Vector3& up)
{
	this->up = up;
	dirty = true;
}
void Camera::SetUpVec(const float x, const float y, const float z)
{
	this->up.x = x;
	this->up.y = y;
	this->up.z = z;
	dirty = true;
}
//���_���W�A�����_���W�A������x�N�g�����Z�b�g
void Camera::SetEyeTargetUpVec(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	dirty = true;
}
void Camera::SetEyeTargetUpVec(const float eyeX, const float eyeY, const float eyeZ,
	const float targetX, const float targetY, const float targetZ,
	const float upX, const float upY, const float upZ)
{
	this->SetEye(eyeX, eyeY, eyeZ);
	this->SetTarget(targetX, targetY, targetZ);
	this->SetUpVec(upX, upY, upZ);
	dirty = true;
}

void Camera::SetPositionAndDistance(const Vector3& pos, const float distance)
{
	SetEye(pos);
	SetDistance(distance);
	dirty = true;
}

void Camera::SetPositionAndDistance(const float posX, const float posY, const float posZ, const float distance)
{
	SetEye(posX, posY, posZ);
	SetDistance(distance);
	dirty = true;
}

void Camera::SetPosition(const Vector3& pos)
{
	SetEye(pos);
	dirty = true;
}

void Camera::SetPosition(const float x, const float y, const float z)
{
	SetEye({ x,y,z });
	dirty = true;
}

void Camera::SetDistance(const float distance)
{
	//���_���W����distance�����O�ɒ����_���W��z�u
	Vector3 newTarget = { eye.x, eye.y, eye.z + distance };
	SetTarget(newTarget);
	dirty = true;
}

void Camera::SetRotation(const Vector3& vector)
{
	rotation.x = vector.x;
	rotation.y = vector.y;
	rotation.z = vector.z;

	dirty = true;
}

void Camera::SetRotation(const float pitch, const float yaw, const float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;

	dirty = true;
}

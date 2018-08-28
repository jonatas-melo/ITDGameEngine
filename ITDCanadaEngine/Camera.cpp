#include "pch.h"
#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

static const XMVECTORF32 START_POSITION = {0.f , 2.5f, 2.f, 0.f};
static const float ROTATION_GAIN = 0.7f;
static const float MOVEMENT_GAIN = 0.5f;
static const Vector3 offset = Vector3(0.f, 0.f, 0.5f);
static const float pitchLimit = XM_PIDIV2 - 0.01f;

Camera::Camera()
{
	position = START_POSITION.v;
	pitch = 0.f;
	yaw = 0.f;
}

Camera::~Camera()
{
}

void Camera::Tilt(float direction)
{
	pitch += ROTATION_GAIN * direction;
	// Limit pitch to straight up or straight down to avoid gimbal lock
	pitch = std::max(-pitchLimit, pitch);
	pitch = std::min(pitchLimit, pitch);
}

void Camera::Pan(float direction)
{
	yaw += ROTATION_GAIN * direction;

	// keep longitude in sane range by wrapping
	if (yaw > XM_PI)
	{
		yaw -= XM_PI * 2.0f;
	}
	else if (yaw < -XM_PI)
	{
		yaw += XM_PI * 2.0f;
	}
}


void Camera::GetViewMatrix(Matrix &outMatrix)
{
	// Vector3 lookAt = position + Vector3(x, y, z);
	Vector3 lookAt = position - Vector3::Transform(offset, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.f));

	outMatrix = Matrix::CreateLookAt(position, lookAt, Vector3::Up);
}

void Camera::ResetCamera()
{
	position = START_POSITION.v;
	pitch = 0.f;
	yaw = 0.f;
}

void Camera::UpdatePosition(Vector3 direction)
{
	auto rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.f);
	// Rotating the movement so its forward points to the same direction as the camera
	direction = Vector3::Transform(direction, rotation);
	direction *= MOVEMENT_GAIN;
	position += direction;
}

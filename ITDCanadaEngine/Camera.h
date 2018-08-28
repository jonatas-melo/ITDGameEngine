#pragma once
class Camera
{
public:
	Camera();
	~Camera();
	void Tilt(float direction);
	void Pan(float direction);

	DirectX::SimpleMath::Vector3 position;
	float pitch;
	float yaw;

	void GetViewMatrix(DirectX::SimpleMath::Matrix& outMatrix);
	void ResetCamera();
	void UpdatePosition(DirectX::SimpleMath::Vector3 move);
};


#pragma once

#include<Renderer/Core/Math.h>
#include<Renderer/Core/Camera.h>

class FreelookCamera {
public:
	float moveSpeed = 5.0f;
	float sensitivity = 0.2f;
public:
	FreelookCamera();
	zRender::Camera& GetCamera() { return m_Camera; }

	void Update();

private:
	zRender::Camera m_Camera;
	float yaw;
	float pitch;
};
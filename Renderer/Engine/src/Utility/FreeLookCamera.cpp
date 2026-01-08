#include"Utility/FreelookCamera.h"

#include<DirectXMath.h>

#include<imgui/imgui.h>
#include <cstdio>

using namespace DirectX;

constexpr float DEG2RAD = 3.14159265 / 180.0f;

FreelookCamera::FreelookCamera() 
	: pitch(0), yaw(0), m_Camera(1536, 793, 60.0f, 0.01f, 100.0f) 
{
	m_Camera.renderMode = zRender::Camera::Perspective;
}

void FreelookCamera::Update() {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 delta = ImGui::GetIO().MouseDelta;
	bool allowMouseLook = !io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left);

	if (allowMouseLook) {
		yaw += delta.x * sensitivity;
		pitch -= delta.y * sensitivity;

		if (pitch > 90) pitch = 90;
		else if (pitch < -90) pitch = -90;

		if (yaw > 360.0f) yaw -= 360.0f;
		if (yaw < 0.0f)   yaw += 360.0f;

		float yawRad = yaw * DEG2RAD;
		float pitchRad = pitch * DEG2RAD;

		m_Camera.forward.x = cos(pitchRad) * sin(yawRad);
		m_Camera.forward.y = sin(pitchRad);
		m_Camera.forward.z = cos(pitchRad) * cos(yawRad);
		zRender::vec3::normalize(m_Camera.forward);
	}

	XMVECTOR fw = XMVectorSet(m_Camera.forward.x, m_Camera.forward.y, m_Camera.forward.z, 0);
	XMVECTOR wUp = XMVectorSet(0, 1, 0, 0);

	XMVECTOR rg = -XMVector3Cross(fw, wUp);

	zRender::vec3 right = { XMVectorGetX(rg), XMVectorGetY(rg), XMVectorGetZ(rg) };
	zRender::vec3::normalize(right);

	if (ImGui::IsKeyDown(ImGuiKey_W)) {
		m_Camera.position = m_Camera.position + m_Camera.forward * moveSpeed * io.DeltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_S)) {
		m_Camera.position = m_Camera.position - m_Camera.forward * moveSpeed * io.DeltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_A)) {
		m_Camera.position = m_Camera.position - right * moveSpeed * io.DeltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_D)) {
		m_Camera.position = m_Camera.position + right * moveSpeed * io.DeltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_Q)) {
		zRender::vec3 pos = m_Camera.position;
		m_Camera.position = { pos.x, pos.y + 1 * io.DeltaTime * moveSpeed, pos.z };
	}
	if (ImGui::IsKeyDown(ImGuiKey_E)) {
		zRender::vec3 pos = m_Camera.position;
		m_Camera.position = { pos.x, pos.y - 1 * io.DeltaTime * moveSpeed, pos.z };
	}
}
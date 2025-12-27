#pragma once

#include<DirectXMath.h>

#include"Math.h"

namespace zRender {
	struct Camera {
	public:
		vec3 up;
		vec3 forward;
		vec3 position;
		float aspectRatio, fov, nearPlane, farPlane;

		Camera(float aspectRatio = 1.0f, float fov = 60.0f, float nearPlane = 0.01f, float farPlane = 100.0f);
		DirectX::XMMATRIX ViewProjMatrix() const;
		DirectX::XMMATRIX ProjMatrix() const;
		DirectX::XMMATRIX ViewMatrix() const;
	};
}
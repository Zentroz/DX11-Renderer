#pragma once

#include<DirectXMath.h>

#include"Renderer/Core/Math.h"

namespace zRender {
	struct Camera {
	public:
		enum RenderMode 
		{ 
			Perspective, Orthographic 
		} renderMode;
		vec3 up, forward, position;
		float width, height, fov, nearPlane, farPlane;

	public:
		Camera(float width = 1920, float height = 1080, float fov = 60.0f, float nearPlane = 0.01f, float farPlane = 100.0f);
		DirectX::XMMATRIX ViewProjMatrix() const;
		DirectX::XMMATRIX ProjMatrix() const;
		DirectX::XMMATRIX ViewMatrix() const;
	};
}
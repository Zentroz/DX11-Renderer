#include"Renderer/Render/Camera.h"

using namespace DirectX;

namespace zRender {
	Camera::Camera(float width, float height, float fov, float nearPlane, float farPlane) :
		fov(fov), width(width), height(height), nearPlane(nearPlane), farPlane(farPlane), renderMode(Perspective)
	{
		position = { 0, 0, -5 };
		up = { 0, 1, 0 };
		forward = { 0, 0, 1 };
	}

	XMMATRIX Camera::ViewProjMatrix() const {
		return ViewMatrix() * ProjMatrix();
	}

	XMMATRIX Camera::ViewMatrix() const {
		XMVECTOR forwardVec = XMVectorSet(forward.x, forward.y, forward.z, 0);

		XMVECTOR upVec = XMVectorSet(up.x, up.y, up.z, 1);
		XMVECTOR positionVec = XMVectorSet(position.x, position.y, position.z, 1);
		XMVECTOR targetVec = XMVectorSet(position.x, position.y, position.z, 1) + XMVector3Normalize(forwardVec);

		XMMATRIX view = XMMatrixLookAtLH(positionVec, targetVec, upVec);

		return view;
	}

	XMMATRIX Camera::ProjMatrix() const {
		XMMATRIX proj = XMMatrixIdentity();

		switch (renderMode) {
		case Perspective:
			proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), width / height, nearPlane, farPlane);
			break;
		case Orthographic:
			proj = XMMatrixOrthographicLH(width, height, nearPlane, farPlane);
			break;
		}

		return XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), width / height, nearPlane, farPlane);
	}

}
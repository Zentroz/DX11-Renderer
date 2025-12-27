#include"Core/Camera.h"

using namespace DirectX;

namespace zRender {
	Camera::Camera(float aspectRatio, float fov, float nearPlane, float farPlane) :
		fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
	{
		position = { 0, 0, -5 };
		up = { 0, 1, 0 };
		forward = { 0, 0, 1 };
	}

	XMMATRIX Camera::ViewProjMatrix() const {
		XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);

		XMVECTOR upVec = XMVectorSet(up.x, up.y, up.z, 1);
		XMVECTOR positionVec = XMVectorSet(position.x, position.y, position.z, 1);
		XMVECTOR forwardVec = XMVectorSet(position.x + forward.x, position.y + forward.y, position.z + forward.z, 0);
		//XMVECTOR forwardVec = XMVectorSet(0, 0, 0, 0);

		XMMATRIX view = XMMatrixLookAtLH(positionVec, forwardVec, upVec);

		return view * proj;
	}

	XMMATRIX Camera::ViewMatrix() const {
		XMVECTOR upVec = XMVectorSet(up.x, up.y, up.z, 1);
		XMVECTOR positionVec = XMVectorSet(position.x, position.y, position.z, 1);
		XMVECTOR forwardVec = XMVectorSet(position.x + forward.x, position.y + forward.y, position.z + forward.z, 0);
		//XMVECTOR forwardVec = XMVectorSet(0, 0, 0, 0);

		XMMATRIX view = XMMatrixLookAtLH(positionVec, forwardVec, upVec);

		return view;
	}

	XMMATRIX Camera::ProjMatrix() const {
		XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);

		return proj;
	}

}
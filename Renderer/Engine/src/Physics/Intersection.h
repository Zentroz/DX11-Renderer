#pragma once

#include<Renderer/Core/Math.h>

bool IsRayIntersecting(
    const zRender::vec3& rayOrigin,
    const zRender::vec3& rayDir,
    const zRender::vec3& v0,
    const zRender::vec3& v1,
    const zRender::vec3& v2,
    float& t,
    float& u,
    float& v
);
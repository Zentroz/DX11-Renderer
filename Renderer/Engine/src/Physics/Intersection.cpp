#include"Intersection.h"

using namespace zRender;

bool IsRayIntersecting(
    const zRender::vec3& rayOrigin,
    const zRender::vec3& rayDir,
    const zRender::vec3& v0,
    const zRender::vec3& v1,
    const zRender::vec3& v2,
    float& t,
    float& u,
    float& v
) {
    const float EPSILON = 1e-8f;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 h = cross(rayDir, edge2);
    float a = dot(edge1, h);

    //if (a > -EPSILON && a < EPSILON)
        //return false; // Ray is parallel to triangle
    if (fabs(a) < EPSILON)
        return false;


    float f = 1.0f / a;
    vec3 s = rayOrigin - v0;
    u = f * dot(s, h);

    if (u < 0.0f || u > 1.0f)
        return false;

    vec3 q = cross(s, edge1);
    v = f * dot(rayDir, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * dot(edge2, q);

    if (t > EPSILON)
        return true; // Intersection!
    else
        return false; // Line intersection but not a ray hit
}
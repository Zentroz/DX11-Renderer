#pragma once

#include<math.h>

namespace zRender{
	struct vec2 {
		float x, y;
	};
	struct vec3 {
		float x, y, z;

		friend vec3 operator+(const vec3& lhs, const vec3& rhs) {
			return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
		}
		friend vec3 operator-(const vec3& lhs, const vec3& rhs) {
			return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
		}
		vec3 operator*(float scalar) const {
			return { this->x * scalar, this->y * scalar, this->z * scalar };
		}
	};
	struct vec4 {
		float x, y, z, w;
	};

	struct int4 { 
		int x, y, z, w;
	};

	inline float sqrDist(const vec3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
	inline float sqrDist(const vec4& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
	}
	inline vec3 normalize(const vec3& v) {
		float length = sqrt(sqrDist(v));
		return vec3(v.x / length, v.y / length, v.z / length);
	}
	inline vec4 normalize(const vec4& v) {
		float length = sqrt(sqrDist(v));
		return vec4(v.x / length, v.y / length, v.z / length, v.w / length);
	}
	inline vec3 cross(const vec3& a, const vec3& b) {
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}
	inline float dot(const vec3& a, const vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
}
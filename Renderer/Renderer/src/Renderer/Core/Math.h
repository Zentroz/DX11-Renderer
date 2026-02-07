#pragma once

#include<math.h>

namespace zRender{
	struct vec2 {
		float x, y;
	};
	struct vec3 {
		float x, y, z;

		float distance() const {
			return sqrt(x * x + y * y + z * z);
		}

		static void normalize(vec3& vector) {
			float dist = vector.distance();
			vector.x /= dist; 
			vector.y /= dist; 
			vector.z /= dist;
		}

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

	inline float sqrDistVec(const vec3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
	inline float sqrDistVec(const vec4& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
	}
	inline vec3 normalizeVec(const vec3& v) {
		float length = sqrt(sqrDistVec(v));
		return vec3(v.x / length, v.y / length, v.z / length);
	}
	inline vec4 normalizeVec(const vec4& v) {
		float length = sqrt(sqrDistVec(v));
		return vec4(v.x / length, v.y / length, v.z / length, v.w / length);
	}
}
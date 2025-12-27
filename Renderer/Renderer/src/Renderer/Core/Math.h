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
}
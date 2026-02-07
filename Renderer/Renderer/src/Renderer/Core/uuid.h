#pragma once

#ifndef UUID_GENERATOR_H
#define UUID_GENERATOR_H

#include <random>
#include <string>
#include <sstream>
#include <iomanip>

class uuid {
public:
	uuid() = default;
	const std::string& get() const;

	bool operator==(const uuid& other) const {
		return (m_UID == other.get());
	}

	bool isNull() const;

	static uuid Build();

protected:
	void assign(const std::string& id);
private:
	std::string m_UID = "";
};

namespace std {
	template <>
	struct hash<uuid> {
		size_t operator()(const uuid& k) const {
			// Combine hashes of member variables
			return std::hash<std::string>()(k.get());
		}
	};
}

#endif
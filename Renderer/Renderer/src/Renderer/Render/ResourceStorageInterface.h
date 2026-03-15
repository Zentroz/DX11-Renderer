#pragma once

#include<wrl.h>
#include<unordered_map>
#include"Renderer/Core/Handles.h"

struct IResourceStorage {
	virtual ~IResourceStorage() = default;

protected:
	// Maps
	template<typename T>
	using ResourceMap = std::unordered_map<uuid, T>;
	template<typename T>
	using ResourceMapSharedPtr = std::unordered_map<uuid, std::shared_ptr<T>>;
	template<typename T>
	using ResourceMapComPtr = std::unordered_map<uuid, Microsoft::WRL::ComPtr<T>>;

	// Util Functions
	void ThrowMissingResource(Handle handle, const std::string& resourceName) {
		printf("A resource of type: %s, is missing with handle : %s \n", resourceName.c_str(), handle.get().c_str());
		throw std::exception();
	}
};
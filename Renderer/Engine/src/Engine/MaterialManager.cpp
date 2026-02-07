#include"Engine/MaterialManager.h"

Handle MaterialManager::Add(const EngineMaterial& mat) {
	Handle h = uuid::Build();
	m_MaterialCounts++;

	m_Materials[h] = mat;

	return h;
}

EngineMaterial MaterialManager::Get(Handle handle) {
	if (handle.isNull() || !m_Materials.contains(handle)) return {};

	return m_Materials[handle];
}
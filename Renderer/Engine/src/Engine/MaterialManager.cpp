#include"Engine/MaterialManager.h"

Handle MaterialManager::Add(const EngineMaterial& mat) {
	Handle h = m_MaterialCounts + 1;
	m_MaterialCounts++;

	m_Materials[h] = mat;

	return h;
}

EngineMaterial MaterialManager::Get(Handle handle) {
	if (!m_Materials.contains(handle)) return {};

	return m_Materials[handle];
}
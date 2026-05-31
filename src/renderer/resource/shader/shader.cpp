#include "shader.h"

ShaderManager::ShaderManager(IRHI* rhi) : m_rhi(rhi) {}
ShaderManager::~ShaderManager() {}

IShader* ShaderManager::create(const ShaderDesc&) { return nullptr; }
void ShaderManager::destroy(IShader*) {}

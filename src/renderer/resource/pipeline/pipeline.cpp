#include "pipeline.h"

PipelineManager::PipelineManager(IRHI* rhi) : m_rhi(rhi) {}
PipelineManager::~PipelineManager() {}

IPipeline* PipelineManager::createGraphics(const GraphicsPipelineDesc&) { return nullptr; }
IPipeline* PipelineManager::createCompute(const ComputePipelineDesc&) { return nullptr; }
void PipelineManager::destroy(IPipeline*) {}

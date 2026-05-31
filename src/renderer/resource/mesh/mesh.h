#pragma once

#include "../../rhi/rhi_enums.h"
#include <cstdint>

class IBuffer;

class Mesh {
public:
    Mesh() = default;
    ~Mesh() = default;

    void setVertexBuffer(IBuffer* buffer, uint32_t stride) {
        m_vertexBuffer = buffer;
        m_vertexStride = stride;
    }
    void setIndexBuffer(IBuffer* buffer, IndexType type) {
        m_indexBuffer = buffer;
        m_indexType = type;
    }

    IBuffer* getVertexBuffer() const { return m_vertexBuffer; }
    IBuffer* getIndexBuffer() const { return m_indexBuffer; }
    uint32_t getVertexStride() const { return m_vertexStride; }
    IndexType getIndexType() const { return m_indexType; }

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

private:
    IBuffer* m_vertexBuffer = nullptr;
    IBuffer* m_indexBuffer = nullptr;
    uint32_t m_vertexStride = 0;
    IndexType m_indexType = IndexType::UINT32;
};

#pragma once

#include <cstdint>

enum class Format : uint32_t {
    UNKNOWN = 0,

    R8_UNORM,
    R8G8_UNORM,
    R8G8B8A8_UNORM,
    R8G8B8A8_SRGB,
    B8G8R8A8_UNORM,
    B8G8R8A8_SRGB,

    R32_SFLOAT,
    R32G32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32B32A32_SFLOAT,

    D16_UNORM,
    D32_SFLOAT,
    D24_UNORM_S8_UINT,
    D32_SFLOAT_S8_UINT,
};

enum class DescriptorType : uint32_t {
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    SAMPLER,
    COMBINED_IMAGE_SAMPLER,
    SAMPLED_IMAGE,
    STORAGE_IMAGE,
};

enum class ShaderStage : uint32_t {
    NONE    = 0,
    VERTEX  = 1 << 0,
    FRAGMENT = 1 << 1,
    COMPUTE = 1 << 2,
    ALL_GRAPHICS = VERTEX | FRAGMENT,
};

enum class BufferUsage : uint32_t {
    VERTEX       = 1 << 0,
    INDEX        = 1 << 1,
    UNIFORM      = 1 << 2,
    STORAGE      = 1 << 3,
    INDIRECT     = 1 << 4,
    TRANSFER_SRC = 1 << 5,
    TRANSFER_DST = 1 << 6,
};

inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
    return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool operator&(BufferUsage a, BufferUsage b) {
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

enum class ImageUsage : uint32_t {
    SAMPLED           = 1 << 0,
    STORAGE           = 1 << 1,
    COLOR_ATTACHMENT  = 1 << 2,
    DEPTH_STENCIL     = 1 << 3,
    TRANSFER_SRC      = 1 << 4,
    TRANSFER_DST      = 1 << 5,
};

inline ImageUsage operator|(ImageUsage a, ImageUsage b) {
    return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool operator&(ImageUsage a, ImageUsage b) {
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

enum class ImageLayout : uint32_t {
    UNDEFINED,
    GENERAL,
    COLOR_ATTACHMENT,
    DEPTH_STENCIL,
    DEPTH_STENCIL_READ_ONLY,
    SHADER_READ,
    TRANSFER_SRC,
    TRANSFER_DST,
    PRESENT,
};

enum class ImageAspect : uint32_t {
    NONE   = 0,
    COLOR  = 1 << 0,
    DEPTH  = 1 << 1,
    STENCIL = 1 << 2,
};

inline ImageAspect operator|(ImageAspect a, ImageAspect b) {
    return static_cast<ImageAspect>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool operator&(ImageAspect a, ImageAspect b) {
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

enum class CullMode : uint32_t {
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK,
};

enum class FrontFace : uint32_t {
    COUNTER_CLOCKWISE,
    CLOCKWISE,
};

enum class PolygonMode : uint32_t {
    FILL,
    LINE,
    POINT,
};

enum class CompareOp : uint32_t {
    NEVER,
    LESS,
    EQUAL,
    LESS_OR_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_OR_EQUAL,
    ALWAYS,
};

enum class BlendFactor : uint32_t {
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
};

enum class BlendOp : uint32_t {
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

enum class IndexType : uint32_t {
    UINT16,
    UINT32,
};

enum class Filter : uint32_t {
    NEAREST,
    LINEAR,
};

enum class MipmapMode : uint32_t {
    NEAREST,
    LINEAR,
};

enum class AddressMode : uint32_t {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class ResourceState : uint32_t {
    UNDEFINED,
    VERTEX_BUFFER,
    INDEX_BUFFER,
    UNIFORM_BUFFER,
    SHADER_RESOURCE,
    UNORDERED_ACCESS,
    RENDER_TARGET,
    DEPTH_WRITE,
    DEPTH_READ,
    COPY_SRC,
    COPY_DST,
    PRESENT,
    COMMON,
};

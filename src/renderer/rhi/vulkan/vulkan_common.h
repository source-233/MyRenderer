#pragma once

#include <volk.h>
#include "../rhi_enums.h"
#include <cassert>
#include <cstdio>

#define VK_CHECK(call) do { \
    VkResult _result = (call); \
    if (_result != VK_SUCCESS) { \
        fprintf(stderr, "Vulkan error %d at %s:%d\n", _result, __FILE__, __LINE__); \
        assert(false && "VK_CHECK failed"); \
    } \
} while(0)

inline VkFormat formatToVk(Format format) {
    switch (format) {
        case Format::R8_UNORM:          return VK_FORMAT_R8_UNORM;
        case Format::R8G8_UNORM:        return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8B8A8_UNORM:    return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SRGB:     return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::B8G8R8A8_UNORM:    return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::B8G8R8A8_SRGB:     return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::R32_SFLOAT:        return VK_FORMAT_R32_SFLOAT;
        case Format::R32G32_SFLOAT:     return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32G32B32_SFLOAT:  return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::D16_UNORM:         return VK_FORMAT_D16_UNORM;
        case Format::D32_SFLOAT:        return VK_FORMAT_D32_SFLOAT;
        case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        default:                        return VK_FORMAT_UNDEFINED;
    }
}

inline VkDescriptorType descriptorTypeToVk(DescriptorType type) {
    switch (type) {
        case DescriptorType::UNIFORM_BUFFER:        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::STORAGE_BUFFER:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorType::SAMPLER:                return VK_DESCRIPTOR_TYPE_SAMPLER;
        case DescriptorType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::SAMPLED_IMAGE:          return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case DescriptorType::STORAGE_IMAGE:          return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

inline VkShaderStageFlagBits shaderStageToVk(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::VERTEX:   return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::COMPUTE:  return VK_SHADER_STAGE_COMPUTE_BIT;
        default:                    return VK_SHADER_STAGE_ALL;
    }
}

inline VkBufferUsageFlags bufferUsageToVk(BufferUsage usage) {
    VkBufferUsageFlags flags = 0;
    if (usage & BufferUsage::VERTEX)       flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (usage & BufferUsage::INDEX)        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (usage & BufferUsage::UNIFORM)      flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (usage & BufferUsage::STORAGE)      flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (usage & BufferUsage::INDIRECT)     flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (usage & BufferUsage::TRANSFER_SRC) flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (usage & BufferUsage::TRANSFER_DST) flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    return flags;
}

inline VkImageUsageFlags imageUsageToVk(ImageUsage usage) {
    VkImageUsageFlags flags = 0;
    if (usage & ImageUsage::SAMPLED)           flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (usage & ImageUsage::STORAGE)           flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (usage & ImageUsage::COLOR_ATTACHMENT)  flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (usage & ImageUsage::DEPTH_STENCIL)     flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (usage & ImageUsage::TRANSFER_SRC)      flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (usage & ImageUsage::TRANSFER_DST)      flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return flags;
}

inline VkImageLayout imageLayoutToVk(ImageLayout layout) {
    switch (layout) {
        case ImageLayout::UNDEFINED:            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::GENERAL:              return VK_IMAGE_LAYOUT_GENERAL;
        case ImageLayout::COLOR_ATTACHMENT:     return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DEPTH_STENCIL:        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageLayout::DEPTH_STENCIL_READ_ONLY: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case ImageLayout::SHADER_READ:          return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageLayout::TRANSFER_SRC:         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TRANSFER_DST:         return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ImageLayout::PRESENT:              return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:                                return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

inline VkImageAspectFlags imageAspectToVk(ImageAspect aspect) {
    VkImageAspectFlags flags = 0;
    if (aspect & ImageAspect::COLOR)   flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (aspect & ImageAspect::DEPTH)   flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (aspect & ImageAspect::STENCIL) flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    return flags;
}

inline VkCullModeFlags cullModeToVk(CullMode mode) {
    switch (mode) {
        case CullMode::NONE:            return VK_CULL_MODE_NONE;
        case CullMode::FRONT:           return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BACK:            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FRONT_AND_BACK:  return VK_CULL_MODE_FRONT_AND_BACK;
        default:                        return VK_CULL_MODE_BACK_BIT;
    }
}

inline VkFrontFace frontFaceToVk(FrontFace face) {
    switch (face) {
        case FrontFace::COUNTER_CLOCKWISE: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case FrontFace::CLOCKWISE:         return VK_FRONT_FACE_CLOCKWISE;
        default:                           return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

inline VkPolygonMode polygonModeToVk(PolygonMode mode) {
    switch (mode) {
        case PolygonMode::FILL:  return VK_POLYGON_MODE_FILL;
        case PolygonMode::LINE:  return VK_POLYGON_MODE_LINE;
        case PolygonMode::POINT: return VK_POLYGON_MODE_POINT;
        default:                 return VK_POLYGON_MODE_FILL;
    }
}

inline VkCompareOp compareOpToVk(CompareOp op) {
    switch (op) {
        case CompareOp::NEVER:            return VK_COMPARE_OP_NEVER;
        case CompareOp::LESS:             return VK_COMPARE_OP_LESS;
        case CompareOp::EQUAL:            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LESS_OR_EQUAL:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::GREATER:          return VK_COMPARE_OP_GREATER;
        case CompareOp::NOT_EQUAL:        return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::ALWAYS:           return VK_COMPARE_OP_ALWAYS;
        default:                          return VK_COMPARE_OP_NEVER;
    }
}

inline VkBlendFactor blendFactorToVk(BlendFactor factor) {
    switch (factor) {
        case BlendFactor::ZERO:                return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::ONE:                 return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SRC_COLOR:           return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR:           return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA:           return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:           return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:                               return VK_BLEND_FACTOR_ZERO;
    }
}

inline VkBlendOp blendOpToVk(BlendOp op) {
    switch (op) {
        case BlendOp::ADD:               return VK_BLEND_OP_ADD;
        case BlendOp::SUBTRACT:          return VK_BLEND_OP_SUBTRACT;
        case BlendOp::REVERSE_SUBTRACT:  return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::MIN:               return VK_BLEND_OP_MIN;
        case BlendOp::MAX:               return VK_BLEND_OP_MAX;
        default:                         return VK_BLEND_OP_ADD;
    }
}

inline VkIndexType indexTypeToVk(IndexType type) {
    switch (type) {
        case IndexType::UINT16: return VK_INDEX_TYPE_UINT16;
        case IndexType::UINT32: return VK_INDEX_TYPE_UINT32;
        default:                return VK_INDEX_TYPE_UINT32;
    }
}

inline VkFilter filterToVk(Filter filter) {
    switch (filter) {
        case Filter::NEAREST: return VK_FILTER_NEAREST;
        case Filter::LINEAR:  return VK_FILTER_LINEAR;
        default:              return VK_FILTER_LINEAR;
    }
}

inline VkSamplerMipmapMode mipmapModeToVk(MipmapMode mode) {
    switch (mode) {
        case MipmapMode::NEAREST: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case MipmapMode::LINEAR:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:                  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

inline VkSamplerAddressMode addressModeToVk(AddressMode mode) {
    switch (mode) {
        case AddressMode::REPEAT:          return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case AddressMode::MIRRORED_REPEAT: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case AddressMode::CLAMP_TO_EDGE:   return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case AddressMode::CLAMP_TO_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:                           return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

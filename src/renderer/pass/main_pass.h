#pragma once

#include "../rhi/rhi_enums.h"
#include <cstdint>

struct MainPassDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    Format colorFormat = Format::R8G8B8A8_UNORM;
    Format depthFormat = Format::D32_SFLOAT;
};

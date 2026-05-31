#pragma once

#include "../rhi/rhi_enums.h"
#include <cstdint>

struct PrePassDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    Format depthFormat = Format::D32_SFLOAT;
};

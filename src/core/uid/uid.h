#pragma once

#include <cstdint>

using UID = uint64_t;

inline UID GenerateUID() {
    static UID counter = 1;
    return counter++;
}

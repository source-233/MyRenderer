#pragma once

#include <cstdint>

#define PROFILE_SCOPE(name)  core::ProfilerScope _p_##__LINE__(name)

namespace core {

class ProfilerScope {
public:
    ProfilerScope(const char* name);
    ~ProfilerScope();

private:
    const char* m_name;
    double      m_start;
};

} // namespace core

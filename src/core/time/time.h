#pragma once

#include <cstdint>

namespace core {

double now();
double deltaTime();

class Timer {
public:
    Timer();
    void    reset();
    double  elapsed() const;

private:
    double  m_start;
};

} // namespace core

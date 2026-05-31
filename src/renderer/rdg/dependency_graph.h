#pragma once

#include <cstdint>
#include <vector>

struct DependencyEdge {
    uint32_t fromPass = 0;
    uint32_t toPass = 0;
};

class DependencyGraph {
public:
    DependencyGraph() = default;
    ~DependencyGraph() = default;

    void build();
    void sort();

private:
    std::vector<DependencyEdge> m_edges;
};

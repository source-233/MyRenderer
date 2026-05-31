#pragma once

#include <vector>
#include <cstdint>

class Node3D;

class Scene {
public:
    Scene() = default;
    ~Scene();

    void init();
    void shutdown();
    void update(float dt);

    void addNode(Node3D* node);
    void removeNode(Node3D* node);

    Node3D* getRoot() const { return m_root; }

private:
    Node3D* m_root = nullptr;
    std::vector<Node3D*> m_nodes;
};

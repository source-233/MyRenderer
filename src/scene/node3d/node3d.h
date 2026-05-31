#pragma once

#include "../node/node.h"
#include "../../core/math/transform.h"
#include <vector>

class Node3D : public Node {
public:
    Node3D() = default;
    ~Node3D() override;

    void update(float dt) override;

    void setTransform(const Transform& t) { m_localTransform = t; }
    const Transform& getLocalTransform() const { return m_localTransform; }
    Matrix4 getWorldMatrix() const;

    void addChild(Node3D* child);
    void removeChild(Node3D* child);

    Node3D* getParent() const { return m_parent; }

private:
    Node3D* m_parent = nullptr;
    std::vector<Node3D*> m_children;
    Transform m_localTransform;
};

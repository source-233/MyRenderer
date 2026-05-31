#pragma once

class Node;

class Inspector {
public:
    Inspector() = default;
    ~Inspector() = default;

    void setTarget(Node* node) { m_target = node; }
    Node* getTarget() const { return m_target; }

    void render();

private:
    Node* m_target = nullptr;
};

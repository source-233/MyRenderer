#pragma once

#include <cstdint>
#include <vector>
#include <string>

class Component;

class Node {
public:
    Node() = default;
    virtual ~Node();

    virtual void update(float dt);

    void addComponent(Component* comp);
    void removeComponent(Component* comp);

    void addChild(Node* child);
    void removeChild(Node* child);

    void setName(const char* name) { m_name = name ? name : ""; }
    const char* getName() const { return m_name.c_str(); }

protected:
    std::string m_name;
    std::vector<Component*> m_components;
};

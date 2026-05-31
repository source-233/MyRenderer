#pragma once

#include "../core/math/math.h"

class Node3D;

class Gizmo {
public:
    Gizmo() = default;
    ~Gizmo() = default;

    enum class Mode { TRANSLATE, ROTATE, SCALE };
    enum class Space { WORLD, LOCAL };

    void update(const Vector3& cameraPos);
    void render();

    void setTarget(Node3D* node) { m_target = node; }
    Node3D* getTarget() const { return m_target; }

    Mode getMode() const { return m_mode; }
    void setMode(Mode mode) { m_mode = mode; }

private:
    Node3D* m_target = nullptr;
    Mode m_mode = Mode::TRANSLATE;
    Space m_space = Space::WORLD;
};

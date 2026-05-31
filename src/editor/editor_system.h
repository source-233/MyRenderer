#pragma once

class Engine;

class EditorSystem {
public:
    EditorSystem() = default;
    ~EditorSystem();

    void init(Engine* engine);
    void shutdown();
    void update(float dt);
    void render();

    bool isOpen() const { return m_open; }
    void toggle() { m_open = !m_open; }

private:
    Engine* m_engine = nullptr;
    bool m_open = true;
};

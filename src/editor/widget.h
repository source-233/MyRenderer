#pragma once

#include <cstdint>
#include <string>

class Widget {
public:
    Widget() = default;
    virtual ~Widget() = default;

    virtual void render() = 0;

    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }

    void setTitle(const char* title) { m_title = title ? title : ""; }
    const char* getTitle() const { return m_title.c_str(); }

private:
    bool m_visible = true;
    std::string m_title = "Widget";
};

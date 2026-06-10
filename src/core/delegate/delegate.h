#pragma once

#include <functional>
#include <vector>

namespace core {

template<typename... Args>
class Delegate {
public:
    using Callback = std::function<void(Args...)>;

    void connect(Callback cb) { m_callbacks.push_back(std::move(cb)); }
    void broadcast(Args... args) {
        for (auto& cb : m_callbacks) {
            if (cb) cb(args...);
        }
    }
    void clear() { m_callbacks.clear(); }

private:
    std::vector<Callback> m_callbacks;
};

} // namespace core

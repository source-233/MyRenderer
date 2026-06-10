#pragma once

struct EngineAPI;

struct IPlugin {
    const char* name;
    bool (*onLoad)(void* instance, EngineAPI* api);
    void (*onUnload)(void* instance);
    void (*onUpdate)(void* instance, float dt);
};

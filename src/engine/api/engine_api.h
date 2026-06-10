#pragma once

#include <cstdint>

struct IRHI;
class Renderer;
class Scene;

struct EngineAPI {
    uint32_t version;
    void*    user_data;

    void    (*log)(void* user, const char* level, const char* msg);
    double  (*getDeltaTime)(void* user);

    IRHI*     (*getRHI)(void* user);
    Renderer* (*getRenderer)(void* user);
    Scene*    (*getScene)(void* user);

    void* (*findPlugin)(void* user, const char* name);
};

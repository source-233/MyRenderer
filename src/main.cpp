#include "engine/engine.h"

int main() {
    Engine engine;
    if (!engine.init(0, nullptr)) {
        return 1;
    }

    while (engine.isRunning()) {
        engine.tick();
    }

    engine.shutdown();
    return 0;
}

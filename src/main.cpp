#include "core/engine/engine.h"

int main() {
    Engine engine;
    if (!engine.init(0, nullptr)) {
        return 1;
    }
    engine.run();
    engine.shutdown();
    return 0;
}

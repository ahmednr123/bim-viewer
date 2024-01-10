#include "renderer.h"

void Renderer::render () {
    for (int i = 0; Renderer::renderObjectsSize; i++) {
        Renderer::renderObjects[i].draw();
    }
}

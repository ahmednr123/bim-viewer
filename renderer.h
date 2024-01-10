#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <vector>
#include "drawable.h"

class Renderer {
public:
    static Drawable* renderObjects;
    static int renderObjectsSize;

    static void addRenderObject (Drawable drawable);
    static void render ();
};

#endif //_RENDERER_H_

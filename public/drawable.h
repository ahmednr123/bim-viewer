#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <vector>
#include <string>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"

class Drawable {
public:
    typedef struct {
        float x, y, z;
        float nx, ny, nz; //normal
    } Vertices;

    typedef struct {
        unsigned int v1, v2, v3;
    } Indices;

    float scale = 1.0f;
    vec3 position;
    vec3 rotation;
    vec4 color;

private:
    int vertices_size;
    Vertices *vertices;

    int indices_size;
    Indices *indices;

    bool isIndicesAdded = false;

    GLuint vertex_buffer_id;
    GLuint indices_buffer_id;
    GLuint vertex_array_obj;

    Shader shader;


public:
    Drawable (Shader shader, Vertices *vertices, int vertices_size)
        : vertices(vertices), shader(shader), vertices_size(vertices_size) { initBuffers(); };
    Drawable (Shader shader, Vertices *vertices, int vertices_size, Indices *indices, int indices_size)
        : vertices(vertices), indices(indices), isIndicesAdded(true), shader(shader),
          vertices_size(vertices_size), indices_size(indices_size) { initBuffers(); };
    ~Drawable ();

    void initBuffers ();
    void setScale (float scale);
    void setPosition (vec3 position);
    void setRotation (vec3 rotation);
    void setColor (vec3 color);

    void bind ();
    void unbind ();

    void draw ();
};

#endif //_DRAWABLE_H

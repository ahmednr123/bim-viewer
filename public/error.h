#ifndef _ERROR_H
#define _ERROR_H

#include <vector>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

static void error_callback (int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void check_error (GLuint shader) {
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);

        GLsizei length;
        glGetShaderInfoLog(shader, log.size(), &length, log.data());

        error_callback(0, log.data());
    }
}

#endif //_ERROR_H

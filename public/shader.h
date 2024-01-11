#ifndef _SHADER_H
#define _SHADER_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "error.h"

class Shader {
public:
    enum VariableType { ATTRIBUTE, UNIFORM };
    enum ValueType { MAT4, VEC3, VEC4 };

private:
    unsigned int program;
    static unsigned int compileShader (unsigned int type, const char* shaderText);
    static unsigned int createShader (const char* vertexShader, const char* fragmentShader);

    typedef struct {
        VariableType type;
        unsigned int location;
        std::string variable;
    } Variable;

    std::unordered_map<std::string, Variable> variables;

public:
    Shader (const char *vs_file, const char *fs_file);
    unsigned int attachValue (VariableType valType, std::string variable);
    void setValue (std::string variable, ValueType valueType, const GLfloat *value);

    void bind ();
    void unbind ();
};

#endif //_SHADER_H

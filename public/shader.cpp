#include "shader.h"

unsigned int
Shader::compileShader (unsigned int type, const char* shaderText) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderText, NULL);
    glCompileShader(shader);
    check_error(shader);

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

    return shader;
};

unsigned int
Shader::createShader (const char* vertexShader, const char* fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
};

Shader::Shader (const char *vs_text, const char *fs_text) {
    program = createShader(vs_text, fs_text);
};

unsigned int
Shader::attachValue (VariableType type, std::string variable) {
    Variable var;
    switch (type) {
        case ATTRIBUTE:
            var.type = ATTRIBUTE;
            var.location = glGetAttribLocation(program, variable.c_str());
            var.variable = variable;
            break;
        case UNIFORM:
            var.type = UNIFORM;
            var.location = glGetUniformLocation(program, variable.c_str());
            var.variable = variable;
            break;
    }
    variables[variable] = var;
    return var.location;
};

void
Shader::setValue (std::string variable, ValueType valueType, const GLfloat *value) {
    auto iter = variables.find(variable);
    if (iter == variables.end()) {
        std::cerr << variable << " has not been attached." << std::endl;
        exit(0);
    }

    Variable var = iter->second;
    switch (valueType) {
        case MAT4:
            glUniformMatrix4fv(var.location, 1, GL_FALSE, value);
            break;
        case VEC3:
            glUniform3fv(var.location, 1, value);
            break;
        case VEC4:
            glUniform3fv(var.location, 1, value);
            break;
    }
};


void
Shader::bind () {
    glUseProgram(program);
}

void
Shader::unbind () {
    glUseProgram(0);
}

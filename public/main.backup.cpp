// emcc main.cpp -o index.html -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -std=c++1z

// base:  https://www.glfw.org/docs/latest/quick.html#quick_example
// ref: https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9

#include <functional>
#include <vector>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    float x, y, z;
    float nx, ny, nz; //normal
} PyramidVertices;

typedef struct {
    unsigned int v1, v2, v3;
} PyramidIndices;

PyramidVertices p_vertices[5] = {
    {-0.5f,-0.5f,-0.5f, 0.f, 0.f, -1.f},
    { 0.5f,-0.5f,-0.5f, 0.f, 0.f, -1.f},
    { 0.0f, 0.5f, 0.0f, 0.f, 0.f, -1.f},
    {-0.5f,-0.5f, 0.5f, 0.f, 0.f, -1.f},
    { 0.5f,-0.5f, 0.5f, 0.f, 0.f, -1.f}
};

PyramidIndices p_indices[4] = {
    {0, 1, 2},
    {1, 4, 2},
    {4, 3, 2},
    {3, 0, 2}
};

static const char *p_vertex_shader_text =
//    "#version 300 es\n"
    "attribute vec3 vPos;\n"
    "attribute vec3 vNormal;\n"
    "varying vec3 FragPos;\n"
    "varying vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(vPos, 1.0));\n"
    "   Normal = vNormal;\n"
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\n";

static const char *p_fragment_shader_text =
//    "#version 300 es\n"
    "precision mediump float;\n"
    "varying vec3 Normal;\n"
    "varying vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "void main()\n"
    "{\n"
    "   float ambientStrength = 0.1;\n"
    "   vec3 ambient = ambientStrength * lightColor;\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = diff * lightColor;\n"
    "   vec3 result = (ambient + diffuse) * objectColor;\n"
    "   gl_FragColor = vec4(result, 1.0);\n"
    "}\n";

static void error_callback (int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback (GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

std::function<void()> loop;
void main_loop() { loop(); }

void check_error (GLuint shader) {
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);

        GLsizei length;
        glGetShaderInfoLog(shader, log.size(), &length, log.data());

        error_callback(0, log.data());
    }
}

static unsigned int CompileShader (unsigned int type, const char* shaderText) {
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
}

static unsigned int CreateShader (const char* vertexShader, const char* fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static float radians (float degrees) {
    return degrees * (M_PI / 100);
}

int main (void) {
    GLint p_vpos_location, p_vnormal_location;
    GLint model_location, view_location, projection_location;
    GLint object_color_location, light_color_location, light_pos_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    auto window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    unsigned int p_program =
        CreateShader(p_vertex_shader_text, p_fragment_shader_text);
    p_vpos_location = glGetAttribLocation(p_program, "vPos");
    p_vnormal_location = glGetAttribLocation(p_program, "vNormal");

    model_location = glGetUniformLocation(p_program, "model");
    projection_location = glGetUniformLocation(p_program, "projection");
    view_location = glGetUniformLocation(p_program, "view");

    object_color_location = glGetUniformLocation(p_program, "objectColor");
    light_color_location = glGetUniformLocation(p_program, "lightColor");
    light_pos_location = glGetUniformLocation(p_program, "lightPos");

    GLuint p_vertex_buffer;
    glGenBuffers(1, &p_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, p_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p_vertices), p_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(p_vpos_location);
    glVertexAttribPointer(p_vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(p_vertices[0]), (void *)0);

    glEnableVertexAttribArray(p_vnormal_location);
    glVertexAttribPointer(p_vnormal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(p_vertices[0]), (void *)(3 * sizeof(float)));

    GLuint p_index_buffer;
    glGenBuffers(1, &p_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(p_indices), p_indices, GL_STATIC_DRAW);

    //GLuint lightPyramidVAO;
    //glGenVertexArrays(p_vnormal_location, &lightPyramidVAO);
    //glBindVertexArray(lightPyramidVAO);

    loop = [&] {
        float ratio;
        int width, height;
        mat4x4 projection;
        mat4x4 model = {
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f }
        };
        mat4x4 view = {
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f }
        };
        mat4x4 temp = {
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f },
            { 1.f, 1.f, 1.f, 1.f }
        };
        vec3 objectColor = {1.f,  1.f,   1.f};
        vec3 lightColor  = {1.f,  1.f,   1.f};
        vec3 lightPos    = {0.f,  0.7f, -0.7f};

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        //printf("width: %d, height: %d\n", width, height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(model);
        mat4x4_rotate_X(model, model, radians(20.0f));

        mat4x4_identity(view);
        float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        //mat4x4_translate(view, 0.0f, 0.0f, -0.3f);
        mat4x4_look_at(view, (vec3 const){camX, 0.0f, camZ}, (vec3 const){0.f, 0.f, 0.f}, (vec3 const){0.f, 1.f, 0.f});

        mat4x4_perspective(projection, 45.0f, ratio, 0.1f, 50.0f);

        glUseProgram(p_program);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat *)model);
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const GLfloat *)projection);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, (const GLfloat *)view);

        glUniform3fv(object_color_location, 1, (const GLfloat *)objectColor);
        glUniform3fv(light_color_location, 1, (const GLfloat *)lightColor);
        glUniform3fv(light_pos_location, 1, (const GLfloat *)lightPos);

        //glBindVertexArray(lightPyramidVAO);
        //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
        glDrawElements(GL_LINE_LOOP, 12, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    emscripten_set_main_loop(main_loop, 0, true);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

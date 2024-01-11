//emcc shader.cpp camera.cpp drawable.cpp main.cpp -o index.html -s "EXPORTED_FUNCTIONS=['_lock','_free_lock','_update_camera_state','_main']" -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -std=c++1z

// base:  https://www.glfw.org/docs/latest/quick.html#quick_example
// ref: https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9
// This is some random text to see if wakatime works fine

#include <string>
#include <functional>
#include <emscripten.h>
#include <vector>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "shader.h"
#include "camera.h"
#include "drawable.h"
#include "bim_content.h"
#include "bim_parser.h"

static bool pos_registered = false;
static vec2 initial_pos = {0.f, 0.f};
static vec2 initial_angles = {0.f, 0.f};
static bool is_mouse_down = false;
static bool locked = false;

extern "C" {
    void lock () {
        printf("LOCKED\n");
        locked = true;
    }
    void free_lock () {
        printf("UNLOCKED\n");
        locked = false;
    }
    void update_camera_state (float fov, float theta, float phi) {
        printf("Updating camera state: %f, %f, %f", fov, theta, phi);
        Camera::fov = fov;
        Camera::theta = theta;
        Camera::phi = phi;
    }
}

static void key_callback (GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static const char *vertex_shader_text =
    "#version 300 es\n"
    "in vec3 vPos;\n"
    "in vec3 vNormal;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(vPos, 1.0));\n"
    "   Normal = mat3(transpose(inverse(model))) * vNormal;\n"
    "   gl_Position = projection * view * model * vec4(vPos, 1.0);\n"
    "}\n";

static const char *fragment_shader_text =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "uniform vec3 viewPos;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   float ambientStrength = 0.6;\n"
    "   vec3 ambient = ambientStrength * lightColor;\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = diff * lightColor;\n"
    "   vec3 viewDir = normalize(viewPos - FragPos);\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);\n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
    "   vec3 specular = 0.5 * spec * lightColor;\n"
    "   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
    "   FragColor = vec4(result, 1.0);\n"
    "}\n";

std::function<void()> loop;
void main_loop() { loop(); }

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (locked)
        return;

    if (is_mouse_down && !pos_registered) {
        initial_pos[0] = xpos;
        initial_pos[1] = ypos;
        initial_angles[0] = Camera::theta;
        initial_angles[1] = Camera::phi;
        pos_registered = true;
    }

    if (is_mouse_down) {
        vec2 curr_pos;
        curr_pos[0] = xpos;
        curr_pos[1] = ypos;
        Camera::trackballCamera(initial_angles, initial_pos, curr_pos);
    }
}

void scroll_callback (GLFWwindow* window, double xoffset, double yoffset) {
    if (locked)
        return;
    Camera::updateFov(yoffset);
}

void cursor_enter_callback (GLFWwindow* window, int entered) {
    if (locked)
        return;

    if (entered) {
    } else {
        is_mouse_down = false;
        pos_registered = false;
        EM_ASM(free_socket_lock());
    }
}

void mouse_button_callback (GLFWwindow* window, int button, int action, int mods) {
    if (locked)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //send_log();
        printf("Mouse pressed\n");
        is_mouse_down = true;
        EM_ASM(acquire_lock());
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //send_log();
        printf("Mouse released\n");
        is_mouse_down = false;
        pos_registered = false;
        EM_ASM(free_socket_lock());
    }
}

int main (void) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    Shader shader = {vertex_shader_text, fragment_shader_text};

    shader.attachValue(Shader::UNIFORM, "lightPos");
    shader.attachValue(Shader::UNIFORM, "objectColor");
    shader.attachValue(Shader::UNIFORM, "lightColor");
    shader.attachValue(Shader::UNIFORM, "projection");
    shader.attachValue(Shader::UNIFORM, "viewPos");
    shader.attachValue(Shader::UNIFORM, "model");
    shader.attachValue(Shader::UNIFORM, "view");

    Limit x_limit = getLimit(bim_json["meshes"], bim_json["elements"], 0);
    Limit y_limit = getLimit(bim_json["meshes"], bim_json["elements"], 1);
    Limit z_limit = getLimit(bim_json["meshes"], bim_json["elements"], 2);

    std::vector<Drawable> objects;
    for (int i = 0; i < bim_json["meshes"].size(); i++) {
        BIMObject obj = getBIMObject2(bim_json, bim_json["meshes"][i], x_limit, y_limit, z_limit);
        if (obj.has_indices) {
            Drawable drawable = {shader, obj.vertices, obj.vertices_size, obj.indices, obj.indices_size};
            drawable.setScale(1.f);
            drawable.setPosition(obj.position);
            drawable.setRotation((vec3){-45.f,0.f,0.f});
            drawable.setColor(obj.color);
            objects.push_back(drawable);
        } else {
            Drawable drawable = {shader, obj.vertices, obj.vertices_size};
            drawable.setScale(1.f);
            drawable.setPosition(obj.position);
            drawable.setRotation((vec3){-45.f,0.f,0.f});
            drawable.setColor(obj.color);
            objects.push_back(drawable);
        }
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);

    shader.bind();

    loop = [&] {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        Camera::updateDimensions(width, height);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (Drawable drawable : objects) {
            drawable.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    emscripten_set_main_loop(main_loop, 0, true);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

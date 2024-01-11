#include "camera.h"
#include "linmath.h"
#include "utils.h"
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

int Camera::width = 800;
int Camera::height = 600;

float Camera::radius = 2.0f;
vec3 Camera::front = {0.f, 0.f, 0.f};
vec3 Camera::up = {0.f, 1.f, 0.f};

float Camera::yaw = -90.f;
float Camera::pitch = 0.f;
float Camera::fov = 45.f;

float Camera::theta = -74.42f;
float Camera::phi = 34.11f;

EM_JS(void, update_camera_state, (float fov, float theta, float phi), {
    if (!shared_data.is_locked) {
        shared_data.camera_state.fov = fov;
        shared_data.camera_state.theta = theta;
        shared_data.camera_state.phi = phi;
        push_camera_state();
    }
});

void
Camera::trackballCamera (vec2 initialAngles, vec2 initialPos, vec2 currPos) {
    // Apply the offset to the latitude and longitude angles
    theta = -((currPos[0] - initialPos[0]) * 0.5f) + initialAngles[0];
    phi = ((currPos[1] - initialPos[1]) * 0.5f) + initialAngles[1];
    phi = std::min(179.f, std::max(0.f, phi));
    update_camera_state(fov, theta, phi);
}

void
Camera::updateDimensions(int width, int height) {
    Camera::width = width;
    Camera::height = height;
}

void
Camera::updateCameraPosition (float camX, float camY, float camZ) {
    //Camera::position[0] = camX;
    //Camera::position[1] = camY;
    //Camera::position[2] = camZ;
}

void Camera::updateFrontVector () {
    vec3 new_front;
    new_front[0] = cos(radians(yaw)) * cos(radians(pitch));
    new_front[1] = sin(radians(pitch));
    new_front[2] = sin(radians(yaw)) * cos(radians(pitch));
    vec3_norm(front, new_front);
}

void Camera::processMouseMovement (float x_offset, float y_offset) {
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (pitch > 89.f) pitch = 89.f;
    if (pitch < -89.f) pitch = -89.f;

    updateFrontVector();
}

void Camera::updateFov (float y_offset) {
    fov -= y_offset;
    if (fov < 1.f)
        fov = 1.f;
    if (fov > 45.f)
        fov = 45.f;

    update_camera_state(fov, theta, phi);
}

void
Camera::populateCameraPosition (vec3 position) {
    position[0] = radius * sin(degrees(theta)) * cos(degrees(phi));
    position[1] = radius * sin(degrees(phi));
    position[2] = radius * cos(degrees(theta)) * cos(degrees(phi));
}

void
Camera::populateViewMatrix (mat4x4 view) {
    mat4x4_identity(view);

    vec3 position;
    populateCameraPosition(position);
    //position[0] = radius * sin(degrees(theta)) * cos(degrees(phi));
    //position[1] = radius * sin(degrees(phi));
    //position[2] = radius * cos(degrees(theta)) * cos(degrees(phi));

    mat4x4_look_at(view, position, front, up);
}

void
Camera::populateProjectionMatrix (mat4x4 projection) {
    mat4x4_perspective(
        projection,
        radians(fov),
        width / (float)height,
        0.1f,
        50.0f
    );
}

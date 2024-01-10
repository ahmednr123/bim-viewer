#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "linmath.h"

class Camera {
private:
    static int width, height;

    static float radius;
    static vec3 front, up;

    static float yaw;
    static float pitch;
    static float fov; //For projection

    static constexpr float sensitivity = 0.1f;
    static void updateFrontVector ();

public:
    static float phi;
    static float theta;

    static void updateFov (float y_offset);
    static void processMouseMovement (float x_offset, float y_offset);

    static void updateDimensions (int width, int height);
    static void updateCameraPosition (float camX, float camY, float camZ);
    static void trackballCamera (vec2 initialAngles, vec2 initialPos, vec2 currPos);

    static void populateViewMatrix (mat4x4 m);
    static void populateProjectionMatrix (mat4x4 m);
    //static void pan (float dx, float dy);
    //static void rotate (float dTheta, float dPhi);
    //static void zoom (float distance);
};

#endif // _CAMERA_H_

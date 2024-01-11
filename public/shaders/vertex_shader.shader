attribute vec3 vPos;
attribute vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 FragPos;
varying vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = vNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}


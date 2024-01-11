#include "drawable.h"
#include "linmath.h"
#include "utils.h"
#include <GL/glext.h>

Drawable::~Drawable () {
    glDeleteBuffers(1, &vertex_buffer_id);
    if (isIndicesAdded)
        glDeleteBuffers(1, &indices_buffer_id);
};

void Drawable::initBuffers () {
    glGenVertexArrays(1, &vertex_array_obj);
    glGenBuffers(1, &vertex_buffer_id);

    glBindVertexArray(vertex_array_obj);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(Vertices), vertices, GL_STATIC_DRAW);

    unsigned int v_pos_loc = shader.attachValue(Shader::ATTRIBUTE, "vPos");
    glEnableVertexAttribArray(v_pos_loc);
    glVertexAttribPointer(v_pos_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);

    unsigned int v_norm_loc = shader.attachValue(Shader::ATTRIBUTE, "vNormal");
    glEnableVertexAttribArray(v_norm_loc);
    glVertexAttribPointer(v_norm_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)(3 * sizeof(float)));

    if (isIndicesAdded) {
        glGenBuffers(1, &indices_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(Indices), indices, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glDisableVertexAttribArray(v_pos_loc);
    glDisableVertexAttribArray(v_norm_loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void Drawable::setScale (float scale) {
    this->scale = scale;
}

void Drawable::setPosition (vec3 position) {
    this->position[0] = position[0];
    this->position[1] = position[1];
    this->position[2] = position[2];
}

void Drawable::setRotation (vec3 rotation) {
    this->rotation[0] = rotation[0];
    this->rotation[1] = rotation[1];
    this->rotation[2] = rotation[2];
}

void Drawable::setColor (vec3 color) {
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
}

void Drawable::bind () {
    //shader.bind();
    //glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBindVertexArray(vertex_array_obj);
    //if (isIndicesAdded)
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_id);
};

void Drawable::unbind () {
    //shader.unbind();
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //if (isIndicesAdded)
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void Drawable::draw () {
    bind();

    vec3 objectColor = {color[0], color[1], color[2]};
    shader.setValue("objectColor", Shader::VEC3, (const GLfloat *)objectColor);
    vec3 lightColor  = {1.f,  1.f,   1.f};
    shader.setValue("lightColor", Shader::VEC3, (const GLfloat *)lightColor);
    vec3 lightPos    = {0.f,  1.f, 0.f};
    shader.setValue("lightPos", Shader::VEC3, (const GLfloat *)lightPos);

    vec3 camera_pos;
    mat4x4 model, view, projection;
    Camera::populateViewMatrix(view);
    shader.setValue("view", Shader::MAT4, (const GLfloat *)view);
    Camera::populateProjectionMatrix(projection);
    shader.setValue("projection", Shader::MAT4, (const GLfloat *)projection);
    Camera::populateCameraPosition(camera_pos);
    shader.setValue("viewPos", Shader::VEC3, (const GLfloat *)camera_pos);

    mat4x4_identity(model);
    //mat4x4_translate(model, position[0], position[1], position[2]);
    mat4x4_rotate_X(model, model, radians(rotation[0]));
    mat4x4_rotate_Y(model, model, radians(rotation[1]));
    mat4x4_rotate_Z(model, model, radians(rotation[2]));
    mat4x4_scale(model, model, scale);
    shader.setValue("model", Shader::MAT4, (const GLfloat *)model);

    if (isIndicesAdded) {
        glDrawElements(GL_TRIANGLES, indices_size * 3, GL_UNSIGNED_INT, nullptr);

        //vec3 lineColor = {0.f, 0.f, 0.f};
        //shader.setValue("objectColor", Shader::VEC3, (const GLfloat *)lineColor);
        //glDrawElements(GL_LINE_STRIP, indices_size * 3, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    }
}

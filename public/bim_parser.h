#ifndef _BIM_PARSER_H_
#define _BIM_PARSER_H_

#include "json.hpp"
#include "drawable.h"
#include "linmath.h"

using json = nlohmann::json;

typedef struct {
    int vertices_size;
    Drawable::Vertices *vertices;

    int indices_size;
    Drawable::Indices *indices;

    vec4 color;
    vec3 position;
    vec3 rotation;

    bool has_indices;
} BIMObject;

//ChatGPT
static void populateRotation (vec3 rotation, float qx, float qy, float qz, float qw) {
    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (qw * qx + qy * qz);
    float cosr_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
    rotation[0] = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2.0f * (qw * qy - qz * qx);
    if (std::abs(sinp) >= 1)
        rotation[1] = std::copysign(3.14159265358979323846 / 2.0f, sinp); // use 90 degrees if out of range
    else
        rotation[1] = std::asin(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (qw * qz + qx * qy);
    float cosy_cosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    rotation[2] = std::atan2(siny_cosp, cosy_cosp);
}

typedef struct {
    float min, mid, max;
} Limit;

//index denotes x, y z
static Limit getLimit (json &meshes, json &elements, int index) {
    Limit limit = {999999.f, 0.f, -9999999.f};
    for (int i = 0; i < meshes.size(); i++) {
        for (int j = 0; j < meshes[i]["coordinates"].size(); j+=3) {
            auto mesh = meshes[i]["coordinates"];
            auto vertex = (float)mesh[j+index];
            if (vertex < limit.min)
                limit.min = vertex;
            if (vertex > limit.max)
                limit.max = vertex;
        }
    }
    limit.mid = (limit.min + limit.max)/2.0f;
    return limit;
}

static float getR (Limit x_limit, Limit y_limit, Limit z_limit) {
    Limit max_limit = x_limit;
    float max = x_limit.max - x_limit.min;

    if (max < y_limit.max - y_limit.min) {
        max = y_limit.max - y_limit.min;
        max_limit = y_limit;
    }

    if (max < z_limit.max - z_limit.min) {
        max = z_limit.max - z_limit.min;
        max_limit = z_limit;
    }

    return 1/((max_limit.mid * -1) + max_limit.max);
}

static void populateNormal (vec3 &normal, json &indices, json &vertices, int index) {
    int v_index = (int)indices[index]*3;
    vec3 v1 = {vertices[v_index], vertices[v_index+1], vertices[v_index+2]};
    v_index = (int)indices[index+1]*3;
    vec3 v2 = {vertices[v_index], vertices[v_index+1], vertices[v_index+2]};
    v_index = (int)indices[index+2]*3;
    vec3 v3 = {vertices[v_index], vertices[v_index+1], vertices[v_index+2]};

    vec3 sub_one;
    vec3_sub(sub_one, v2, v1);
    vec3 sub_two;
    vec3_sub(sub_two, v3, v1);

    vec3 cross_prod;
    vec3_mul_cross(cross_prod, sub_one, sub_two);
    vec3_norm(normal, cross_prod);
}

static BIMObject getBIMObject3 (json &bim_json, json &mesh, Limit x_limit, Limit y_limit, Limit z_limit) {
    BIMObject obj;
    obj.has_indices = false;

    json elements = mesh["elements"];
    for () {
    }
}

static BIMObject getBIMObject2 (json &bim_json, json &mesh, Limit x_limit, Limit y_limit, Limit z_limit) {
    BIMObject obj;
    obj.has_indices = false;

    json vertices = mesh["coordinates"];
    json indices = mesh["indices"];

    int indices_size = mesh["indices"].size();
    obj.vertices_size = indices_size;
    obj.vertices = new Drawable::Vertices[indices_size];

    float r = getR(x_limit, y_limit, z_limit);

    for (int i = 0; i < indices_size; i+=3) {
        vec3 face_normal;
        populateNormal(face_normal, indices, vertices, i);
        for (int j = i; j < i+3; j++) {
            obj.vertices[j].x = ((x_limit.mid * -1.f) + (float)vertices[(int)indices[j]*3]) * r;
            obj.vertices[j].y = ((y_limit.mid * -1.f) + (float)vertices[((int)indices[j]*3)+1]) * r;
            obj.vertices[j].z = ((z_limit.mid * -1.f) + (float)vertices[((int)indices[j]*3)+2]) * r;

            obj.vertices[j].nx = face_normal[0];
            obj.vertices[j].ny = face_normal[1];
            obj.vertices[j].nz = face_normal[2];
        }
    }

    json elements = bim_json["elements"];
    json element = nlohmann::detail::value_t::null;
    for (int i = mesh["mesh_id"]; i < elements.size(); i++) {
        if (elements[i]["mesh_id"] == mesh["mesh_id"]) {
            element = elements[i];
        }
    }

    if (element == nlohmann::detail::value_t::null) {
        std::cerr << "'mesh_id' = " << mesh["mesh_id"] << " not found" << std::endl;
        exit(0);
    }

    obj.position[0] = ((x_limit.mid * -1.f) + (float)element["vector"]["x"]) * r;
    obj.position[1] = ((y_limit.mid * -1.f) + (float)element["vector"]["y"]) * r;
    obj.position[2] = ((z_limit.mid * -1.f) + (float)element["vector"]["z"]) * r;

    obj.color[0] = (float)element["color"]["r"]/255.0f;
    obj.color[1] = (float)element["color"]["g"]/255.0f;
    obj.color[2] = (float)element["color"]["b"]/255.0f;
    obj.color[3] = element["color"]["a"];

    populateRotation(
        obj.rotation,
        element["rotation"]["qx"],
        element["rotation"]["qy"],
        element["rotation"]["qz"],
        element["rotation"]["qw"]
    );

    //IGNORING face_colors

    return obj;
}

static BIMObject getBIMObject (json &bim_json, json &mesh, Limit x_limit, Limit y_limit, Limit z_limit) {
    BIMObject obj;
    obj.has_indices = true;

    int vertices_size = mesh["coordinates"].size();
    obj.vertices_size = vertices_size/3;
    obj.vertices = new Drawable::Vertices[vertices_size/3];

    float r = getR(x_limit, y_limit, z_limit);

    for (int i = 0; i < vertices_size; i+=3) {
        obj.vertices[i/3].x = ((x_limit.mid * -1.f) + (float)mesh["coordinates"][i]) * r;
        obj.vertices[i/3].y = ((y_limit.mid * -1.f) + (float)mesh["coordinates"][i+1]) * r;
        obj.vertices[i/3].z = ((z_limit.mid * -1.f) + (float)mesh["coordinates"][i+2]) * r;

        obj.vertices[i/3].nx = 0.f;
        obj.vertices[i/3].ny = 0.f;
        obj.vertices[i/3].nz = 1.f;
    }

    int indices_size = mesh["indices"].size();
    obj.indices_size = indices_size/3;
    obj.indices = new Drawable::Indices[indices_size/3];

    for (int i = 0; i < indices_size; i+=3) {
        obj.indices[i/3].v1 = mesh["indices"][i];
        obj.indices[i/3].v2 = mesh["indices"][i+1];
        obj.indices[i/3].v3 = mesh["indices"][i+2];
    }

    json elements = bim_json["elements"];
    json element = nlohmann::detail::value_t::null;
    for (int i = mesh["mesh_id"]; i < elements.size(); i++) {
        if (elements[i]["mesh_id"] == mesh["mesh_id"]) {
            element = elements[i];
        }
    }

    if (element == nlohmann::detail::value_t::null) {
        std::cerr << "'mesh_id' = " << mesh["mesh_id"] << " not found" << std::endl;
        exit(0);
    }

    obj.position[0] = element["vector"]["x"];
    obj.position[1] = element["vector"]["y"];
    obj.position[2] = element["vector"]["z"];

    obj.color[0] = (float)element["color"]["r"]/255.0f;
    obj.color[1] = (float)element["color"]["g"]/255.0f;
    obj.color[2] = (float)element["color"]["b"]/255.0f;
    obj.color[3] = element["color"]["a"];

    populateRotation(
        obj.rotation,
        element["rotation"]["qx"],
        element["rotation"]["qy"],
        element["rotation"]["qz"],
        element["rotation"]["qw"]
    );

    //IGNORING face_colors

    return obj;
}

#endif //_BIM_PARSER_H_

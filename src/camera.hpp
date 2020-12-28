#ifndef CAMERA_H
#define CAMERA_H

#include "linalg/vec.hpp"
#include "linalg/mat3.hpp"

class Camera {
    public:
    Camera(float fov, vec2 screen_dim, vec3 position, float xz_rotation) :
        fov(fov), screen_dim(screen_dim), position(position),
        xz_rotation(xz_rotation), rotation_matrix(rotationY(xz_rotation)) {}
    
    void turn(float angle) {
        xz_rotation += angle;
        xz_rotation = fmodf(xz_rotation, 2.0 * M_PI);
        rotation_matrix = rotationY(xz_rotation);
    }

    void move_forward(const vec3& direction) {
        position = position + rotation_matrix * direction;
    }

    vec3 get_ray_dir(const vec2& pixel) const {
        vec2 xy = pixel - screen_dim * 0.5f;

        float cot_half_fov = tan((90.0 - fov * 0.5) * M_PI / 180.0f);
        float z = screen_dim[1] * 0.5 * cot_half_fov;
        
        vec3 dir = normalize(vec3(xy, -z));
        return rotation_matrix * dir;
    }

    vecpack<8, 3> get_ray_dir_simd(const vecpack<8, 2>& pixels) const {
        vecpack<8, 2> xy = pixels - screen_dim * 0.5f;

        float cot_half_fov = tan((90.0 - fov * 0.5) * M_PI / 180.0f);
        float z = screen_dim[1] * 0.5 * cot_half_fov;
        
        vecpack<8, 3> dir = normalize(vecpack<8, 3>({ xy[0], xy[1], vec<8>(-z) }));

        return rotation_matrix * dir;
    }

    vec3 position;
    float xz_rotation;

    private:
    vec2 screen_dim;
    float fov;
    
    mat3 rotation_matrix;

};

#endif
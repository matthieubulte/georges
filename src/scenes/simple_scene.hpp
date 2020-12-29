#ifndef SIMPLE_SCENE_HPP
#define SIMPLE_SCENE_HPP

#include "../transformations.hpp"
#include "../distances.hpp"
#include "scene.hpp"

class SimpleScene : public Scene {
    public:
    vec2 dist_field(const float t, const vec3& p) const;
    vecpack<8, 2> dist_field_simd(const float t, const vecpack<8, 3>& p) const;
    vec3 texture(int texture_id, const vec3& pos) const;
    vecpack<8, 3> texture_simd(const vec<8>& hit_time, const vec<8>& hit_texture) const;
};

vec2 SimpleScene::dist_field(const float t, const vec3& p) const {
    vec3 q;
    float d = 10000.0f;
    
    // floor
    d = dist_plane(vec3(0,1,0), 0, p);

    // sphere
    q = p - vec3(0.0f, 1.0f, 3.0f);
    float d2 = dist_sphere(0.5f, q);
    float ds = smin(d, d2, 0.32);

    return vec2(ds, 1.0);
}

vecpack<8, 2> SimpleScene::dist_field_simd(const float t, const vecpack<8, 3>& p) const {
    vecpack<8, 3> q;
    vec<8> d, d2, ds;
    
    // floor
    d = dist_plane(vec3(0,1,0), 0, p);
    
    // sphere
    q = p - vec3(0.0f, 1.0f, 3.0f);
    d2 = dist_sphere(0.5f, q);
    ds = smin(d, d2, 0.32);
    
    vecpack<8, 2> res;
    res[0] = ds;
    res[1] = 1.0f;

    return res;
}

vec3 SimpleScene::texture(int texture_id, const vec3& pos) const {
    if (texture_id == 2) { // floor
        float x = pos[0] >= 0 ? pos[0] : -pos[0] + 0.5;
        float z = pos[2] >= 0 ? pos[2] : -pos[2] + 0.5;
        return (fmod(x, 1) < 0.5) == (fmod(z, 1) < 0.5) ?
vec3(1,1,1) : vec3(0,0,0);
    } else if (texture_id == 3) { // block
        return vec3(51, 255, 189)/255.0f/5.0;
    } else if (texture_id == 1) { // sphere
        return vec3(255, 189, 51)/255.0f/2.0;
    }
    return vec3(0,1,0);
}

vecpack<8, 3> SimpleScene::texture_simd(const vec<8>& hit_time, const vec<8>& hit_texture) const {
    vecpack<8, 3> col(vec3(0.5, 0.37, 0.1));
    vec<8> col_mask = hit_time >= 0;
    return col_mask * col;
}


#endif
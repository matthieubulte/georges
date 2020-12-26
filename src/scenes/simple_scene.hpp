#ifndef SIMPLE_SCENE_HPP
#define SIMPLE_SCENE_HPP

#include "../transformations.hpp"
#include "../distances.hpp"
#include "scene.hpp"

class SimpleScene : public Scene {
    public:
    vec2 dist_field(const vec3& p) const;
    vecpack<8, 2> dist_field_simd(const vecpack<8, 3>& p) const;
};

vec2 SimpleScene::dist_field(const vec3& p) const {
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

vecpack<8, 2> SimpleScene::dist_field_simd(const vecpack<8, 3>& p) const {
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

#endif
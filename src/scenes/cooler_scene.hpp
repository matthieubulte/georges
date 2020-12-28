
#ifndef SIMPLE_SCENE_HPP
#define SIMPLE_SCENE_HPP

#include "../transformations.hpp"
#include "../distances.hpp"
#include "scene.hpp"

class CoolerScene : public Scene {
    public:
    vec2 dist_field(const float t, const vec3& p) const;
    vecpack<8, 2> dist_field_simd(const float t, const vecpack<8, 3>& p) const;
    vec3 texture(int texture_id, const vec3& pos) const;
    vecpack<8, 3> texture_simd(const vec<8>& hit_time, const vec<8>& hit_texture) const;
};

vec2 CoolerScene::dist_field(const float t, const vec3& p) const {
    vec3 pt, q;
    float d = 100000.0f;
    
    // floor
    pt = translate(vec3(0, 0.075*sin(p[0])*sin(p[2]), 0), p);
    d = dist_plane(vec3(0,1,0), 0, pt);
    vec2 rp = vec2(d, /* texture */ 1);

    // column
    pt = translate(vec3(0,.75,-1.), p);
    pt = repeatX(2, pt);
    d = dist_box(vec3(1, 0.2, 1), pt);

    float crad = 0.15*(1 + 0.1*sin(atan2f(pt[0], pt[2])*20.0f));
    crad -= 0.05*pt[1];
    crad += 0.1*pow(0.1 + 0.1*sin(pt[1]*20), 0.01);

    d = len(vec2(pt[0], pt[2])) - crad;
    d = std::max(d, pt[1]-1.1f);
    d = std::max(d, -pt[1]-5.0f);

    vec2 rb = vec2(d, /* texture */ 3);

    // boxes
    q = translate(vec3(0, 1.1, 0), pt);
    d = dist_box(vec3(0.35, 0.05, 0.35), q);
    vec2 rbb = vec2(d, /* texture */ 3);

    q = translate(vec3(0, -0.7, 0), pt);
    d = dist_box(vec3(0.35, 0.05, 0.35), q);
    rbb = dist_union(rbb, vec2(d, /* texture */ 3));

    // sphere
    float st = 3.0f;
    float y = fmod(t/st, 2.0f)-2*fmod(floorf(t/st), 2.0f)*fmod(t/st,1.0f);
    y = 1 + (2.0f*fmod(floorf(t/st/2),2.0f)-1)*y;

    pt = translate(vec3(-1.5,.5+y/2,-0.5), p);
    float rad = .5;
    d = dist_sphere(rad, pt);
    vec2 rs = vec2(d, /* texture */ 2);
    
    return dist_union(rp, rb, dist_union(rbb, rs));
}

vecpack<8, 2> CoolerScene::dist_field_simd(const float t, const vecpack<8, 3>& p) const {
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
};

vec3 CoolerScene::texture(int texture_id, const vec3& pos) const {
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

vecpack<8, 3> CoolerScene::texture_simd(const vec<8>& hit_time, const vec<8>& hit_texture) const {
    vecpack<8, 3> col(vec3(0.5, 0.37, 0.1));
    vec<8> col_mask = hit_time >= 0;
    return col_mask * col;
}

#endif
#ifndef SCENE_HPP
#define SCENE_HPP

#include "../linalg/vec.hpp"

class Scene {
    public:
    virtual vec2 dist_field(const float t, const vec3& p) const = 0;
    virtual vecpack<8, 2> dist_field_simd(const float t, const vecpack<8, 3>& p) const = 0;
    virtual vec3 texture(int texture_id, const vec3& pos) const = 0;
    virtual vecpack<8, 3> texture_simd(const vec<8>& hit_time, const vec<8>& hit_texture) const = 0;
};

#endif
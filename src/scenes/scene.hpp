#ifndef SCENE_HPP
#define SCENE_HPP

#include "../vec.hpp"

class Scene {
    public:
    virtual vec2 dist_field(const vec3& p) const = 0;
    virtual vecpack<8, 2> dist_field_simd(const vecpack<8, 3>& p) const = 0;
};

#endif
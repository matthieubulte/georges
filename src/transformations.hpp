#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "vec3.hpp"
#include "vec2.hpp"

vec3 translate(const vec3& d, const vec3& p) {
    return p - d;
}

vec3 symX(const vec3& p) {
    vec3 q = p;
    q.x = abs(p.x);
    return q;
}

vec3 symXZ(const vec3& p) {
    vec3 q = p;
    q.x = abs(p.x);
    q.z = abs(p.z);
    return q;
}

vec3 repeatXZ(const vec2& pattern, const vec3& p) {
    return {
        fmodf(p.x + 0.5f * pattern.x, pattern.x) - 0.5f * pattern.x,
        p.y,
        fmodf(p.z + 0.5f * pattern.y, pattern.y) - 0.5f * pattern.y,
    };
}

vec3 repeatXY(const vec2& pattern, const vec3& p) {
    return {
        fmodf(p.x + 0.5f * pattern.x, pattern.x) - 0.5f * pattern.x,
        fmodf(p.y + 0.5f * pattern.y, pattern.y) - 0.5f * pattern.y,
        p.z,
    };
}

vec3 repeatLim(float c, float l, const vec3& p) {
    return p-c*vclamp(apply(p/c, roundf),-l,l);
}

float displacement(const vec3& p) {
    return sin(20*p.x)*sin(20*p.y)*sin(20*p.z);
}

#endif
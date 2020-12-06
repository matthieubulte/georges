#ifndef DISTANCES_H
#define DISTANCES_H

#include "vec3.hpp"
#include "vec2.hpp"

float dist_sphere(float r, const vec3& p) {
    return len(p) - r;
}

float dist_box(const vec3& b, const vec3& p) {
  vec3 q = apply(p, abs) - b;
  return len(vmax(q,0.0f)) + std::min(std::max(q.x,std::max(q.y,q.z)),0.0f);
}

float dist_torus(const vec2& t, const vec3& p) {
  vec2 q = vec2(len(vec2(p.x, p.z))-t.x,p.y);
  return len(q)-t.y;
}

float dist_plane(const vec3& n, float h, const vec3& p) {
  return dot(p,n) + h;
}

vec2 dist_union(const vec2& res1, const vec2& res2) {
    return res1.x < res2.x ? res1 : res2;
}
vec2 dist_union(const vec2& res1, const vec2& res2, const vec2& res3) {
    return dist_union(res1, dist_union(res2, res3));
}

#endif
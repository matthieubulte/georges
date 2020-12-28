#ifndef DISTANCES_H
#define DISTANCES_H

#include <cmath>

#include "linalg/vec.hpp"
#include "linalg/vecpack.hpp"

template<size_t N_vecs>
vec<N_vecs> dist_sphere(float r, const vecpack<N_vecs, 3>& p) {
    return len(p) - r;
}

template<size_t N>
float dist_sphere(float r, const vec<N>& p) {
    return len(p) - r;
}

float dist_plane(const vec3& n, float h, const vec3& p) {
  return dot(p,n) + h;
}

template<size_t N_vecs>
vec<N_vecs> dist_plane(const vec3& n, float h, const vecpack<N_vecs, 3>& p) {
  return dot(p,n) + h;
}

float dist_box(const vec3& b, const vec3& p) {
  vec3 q = apply(p, abs) - b;
  return len(max(q,0.0f)) + std::min(std::max(q[0],std::max(q[1],q[2])),0.0f);
}

float dist_torus(const vec2& t, const vec3& p) {
  vec2 q = vec2(len(vec2(p[0], p[2]))-t[0],p[1]);
  return len(q)-t[1];
}

vec2 dist_union(const vec2& res1, const vec2& res2) {
    return res1[0] < res2[0] ? res1 : res2;
}
vec2 dist_union(const vec2& res1, const vec2& res2, const vec2& res3) {
    return dist_union(res1, dist_union(res2, res3));
}

#endif
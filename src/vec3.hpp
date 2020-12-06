#ifndef VEC3_H
#define VEC3_H

#include <cstdio>
#include <cmath>
#include <iostream>

#include "vec2.hpp"

typedef struct vec3 {
    float x, y, z;
    constexpr vec3() : x(0.0), y(0.0), z(0.0) {}
    constexpr vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    constexpr vec3(const vec2& xy, float z) : x(xy.x), y(xy.y), z(z) {}
    constexpr vec3(float x, const vec2& yz) : x(x), y(yz.x), z(yz.y) {}
} vec3;
vec3 apply(const vec3& v, float (&func) (float)) {
    return { func(v.x), func(v.y), func(v.z)};
}
vec3 operator+(const vec3& lhs, const vec3& rhs) { 
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}
vec3 operator-(const vec3& lhs, const vec3& rhs) { 
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}
vec3 operator-(const vec3& v, float x) { 
    return {v.x - x, v.y - x, v.z - x};
}
vec3 operator-(float x, const vec3& v) { 
    return {x-v.x - x, x-v.y - x, x-v.z};
}
vec3 operator*(float x, const vec3& v) { 
    return {v.x * x, v.y * x, v.z * x};
}
vec3 operator*(const vec3& v, float x) { 
    return {v.x * x, v.y * x, v.z * x};
}
vec3 operator/(const vec3& v, float x) { 
    return {v.x / x, v.y / x, v.z / x};
}
vec3 operator-(const vec3& v) { 
    return -1.0*v;
}

float len(const vec3& v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3 normalize(const vec3& v) {
    float vlen = len(v);
    if (vlen == 0) return v;
    return v/vlen;
}

float dot(const vec3& v, const vec3& w) {
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

vec3 vmax(const vec3& v, float x) {
    return {std::max(v.x, x), std::max(v.y, x), std::max(v.z, x)};
}

vec3 vmin(const vec3& v, float x) {
    return {std::min(v.x, x), std::min(v.y, x), std::min(v.z, x)};
}

vec3 vclamp(const vec3& v, float lo, float hi) {
    return vmin(vmax(v, lo), hi);
}

vec3 pos(const vec3& v) {
    return vec3(std::max(0.0f, v.x), std::max(0.0f, v.y), std::max(0.0f, v.z));
}

vec3 linerp(vec3 lo, vec3 hi, float a) {
    return (1-a)*lo + a*hi;
}

void print_vec3(const vec3& v) {
    using namespace std;
    cout << v.x << ' ' << v.y << ' ' << v.z << endl;
}

#endif
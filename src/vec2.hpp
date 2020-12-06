#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <iostream>

typedef struct vec2 {
    float x, y;
    constexpr vec2() : x(0.0), y(0.0) {}
    constexpr vec2(float x, float y) : x(x), y(y) {}
} vec2;

vec2 operator+(const vec2& lhs, const vec2& rhs) { 
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

vec2 operator-(const vec2& lhs, const vec2& rhs) { 
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
vec2 operator-(const vec2& v, float x) { 
    return {v.x - x, v.y - x};
}
vec2 operator-(float x, const vec2& v) { 
    return {x-v.x - x, x-v.y - x};
}
vec2 operator*(float x, const vec2& v) { 
    return {v.x * x, v.y * x};
}
vec2 operator*(const vec2& v, float x) { 
    return {v.x * x, v.y * x};
}
vec2 operator*(const vec2& v, const vec2& w) { 
    return {v.x * w.x, v.y * w.y};
}
vec2 operator/(const vec2& v, float x) { 
    return {v.x / x, v.y / x};
}
vec2 operator/(const vec2& v, const vec2& w) { 
    return {v.x / w.x, v.y / w.y};
}

vec2 flip(const vec2& v) {
    return {v.y, v.x};
}

vec2 apply(const vec2& v, float (&func) (float)) {
    return { func(v.x), func(v.y)};
}

float len(const vec2& v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

vec2 normalize(const vec2& v) {
    float vlen = len(v);
    if (vlen == 0) return v;
    return v/vlen;
}


void print_vec2(const vec2& v) {
    using namespace std;
    cout << v.x << ' ' << v.y << endl;
}

#endif
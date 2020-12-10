#ifndef MAT3_H
#define MAT3_H

#include <iostream>
#include "vec.hpp"

typedef struct mat3 {
    float data[9];
    constexpr mat3(float x11, float x12, float x13, 
                   float x21, float x22, float x23, 
                   float x31, float x32, float x33) : data{x11, x12, x13, x21, x22, x23, x31, x32, x33} {}
    const float& operator[](int i) const {
        return data[i];
    }
} mat3;

mat3 rotationY(float angle) {
    return mat3(
        cos(angle),  0.0f, sin(angle),
        0.0f,           1.0f, 0.0f,
        -sin(angle), 0.0f, cos(angle)
    );
}

mat3 transpose(const mat3& m) {
    return {m[0], m[3], m[6],
            m[1], m[4], m[7],
            m[2], m[5], m[8]};
}

vec3 operator*(const mat3& m, const vec3& v) { 
    return {m[0]*v[0] + m[1]*v[1] + m[2]*v[2],
            m[3]*v[0] + m[4]*v[1] + m[5]*v[2],
            m[6]*v[0] + m[7]*v[1] + m[8]*v[2]};
}

vec3 operator*(const vec3& v, const mat3& m) { 
    return transpose(m)*v;
}

std::ostream& operator<<(std::ostream& o, const mat3& m) {
    o << m[0] << ' ' << m[1] << ' ' << m[2] << std::endl
      << m[3] << ' ' << m[4] << ' ' << m[5] << std::endl
      << m[6] << ' ' << m[7] << ' ' << m[8] << std::endl;
    return o;
}

#endif
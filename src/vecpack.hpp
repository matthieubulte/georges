#ifndef VECPACK_H
#define VECPACK_H

#include <iostream>
#include <cmath>
#include <array>
#include "vec.hpp"

template<size_t N_vecs, size_t vec_N>
struct vecpack {
    std::array<vec<N_vecs>, vec_N> data;

    // access a specific column
    vec<N_vecs>& operator[](int i) {
        return this->data[i];
    }

    const vec<N_vecs>& operator[](int i) const {
        return this->data[i];
    }
};

template<size_t N_vecs>
vecpack<N_vecs, 3> vecpack3(float x, float y, float z) {
    vecpack<N_vecs, 3> res;
    res[0] = x;
    res[1] = y;
    res[2] = z;
    return res;
}

template<size_t N_vecs>
vecpack<N_vecs, 3> vecpack3(vec3 v) {
    vecpack<N_vecs, 3> res;
    res[0] = v[0];
    res[1] = v[1];
    res[2] = v[2];
    return res;
}

template<size_t N_vecs>
vecpack<N_vecs, 2> vecpack2(float x, float y) {
    vecpack<N_vecs, 2> res;
    res[0] = x;
    res[1] = y;
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator+(const vecpack<N_vecs, vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] + rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator+(const vecpack<N_vecs, vec_N>& lhs, const vec<vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] + rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator+(const vec<vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] + rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator+(float lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs + rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator+(const vecpack<N_vecs, vec_N>& lhs, float rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] + rhs;
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator-(const vecpack<N_vecs, vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] - rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator-(const vecpack<N_vecs, vec_N>& lhs, const vec<vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] - rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator-(const vec<vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] - rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator-(float lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs - rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator-(const vecpack<N_vecs, vec_N>& lhs, float rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] - rhs;
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vecpack<N_vecs, vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vecpack<N_vecs, vec_N>& lhs, const vec<vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vec<vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vec<N_vecs>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++)
        for (auto j = 0; j < N_vecs; j++)
            res[i][j] = lhs[j] * rhs[i][j];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(float lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vecpack<N_vecs, vec_N>& lhs, float rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] * rhs;
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vecpack<N_vecs, vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] / rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vecpack<N_vecs, vec_N>& lhs, const vec<vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] / rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vec<vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] / rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vec<N_vecs>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++)
        for (auto j = 0; j < N_vecs; j++)
            res[i][j] = lhs[j] / rhs[i][j];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vecpack<N_vecs, vec_N>& lhs, const vec<N_vecs>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++)
        for (auto j = 0; j < N_vecs; j++)
            res[i][j] = lhs[i][j] / rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(float lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs / rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator/(const vecpack<N_vecs, vec_N>& lhs, float rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs[i] / rhs;
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> apply(const vecpack<N_vecs, vec_N>& v, float (&func) (float)) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = func(v[i]);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> vmax(const vecpack<N_vecs, vec_N>& v, float x) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = std::max(v[i], x);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> vclamp(const vecpack<N_vecs, vec_N>& v, float lo, float hi) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = std::clamp(v[i], lo, hi);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vec<N_vecs> len(const vecpack<N_vecs, vec_N>& v) { 
    return sqrt(dot(v, v));
}

template<size_t N_vecs, size_t vec_N>
vec<N_vecs> dot(const vecpack<N_vecs, vec_N>& lhs, const vecpack<N_vecs, vec_N>& rhs) { 
    vec<N_vecs> res(0.0f);
    for (auto i = 0; i < vec_N; i++) res = res + lhs[i] * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vec<N_vecs> dot(const vecpack<N_vecs, vec_N>& lhs, const vec<vec_N>& rhs) { 
    vec<N_vecs> res(0.0f);
    for (auto i = 0; i < vec_N; i++) res = res + lhs[i] * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> normalize(const vecpack<N_vecs, vec_N>& v) { 
    vec<N_vecs> vlen = len(v);
    // if (vlen == 0) return v; TODO
    return v/vlen;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> interp(const vecpack<N_vecs, vec_N>& v, const vecpack<N_vecs, vec_N>& w, float a) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = a * v[i] + (1 - a)*w[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> pow(const vecpack<N_vecs, vec_N>& v, float a) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = pow(v[i], a);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> pow(float a, const vecpack<N_vecs, vec_N>& v) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = pow(a, v[i]);
    return res;
}

#endif
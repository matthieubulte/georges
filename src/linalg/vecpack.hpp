#ifndef VECPACK_H
#define VECPACK_H

#include <iostream>
#include <cmath>
#include <array>
#include "vec.hpp"

template<size_t N_vecs, size_t vec_N>
struct vecpack {
    std::array<vec<N_vecs>, vec_N> data;

    constexpr vecpack() {}

    constexpr vecpack(std::array<vec<N_vecs>, vec_N> data) : data(data) {}

    constexpr vecpack(vec<vec_N> v) {
        for (auto i = 0; i < vec_N; i++) this->data[i] = v[i];
    }

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
        res[i] = lhs * rhs[i];
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> operator*(const vecpack<N_vecs, vec_N>& lhs, const vec<N_vecs>& rhs) { 
    return rhs * lhs;
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
vecpack<N_vecs, vec_N> operator*(const vec<N_vecs>& lhs, vec<vec_N> rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = lhs * rhs[i];
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
        res[i] = lhs[i] / rhs;
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
vecpack<N_vecs, vec_N> max(const vecpack<N_vecs, vec_N>& v, float x) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = max(v[i], x);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> min(const vecpack<N_vecs, vec_N>& v, float x) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = min(v[i], x);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> clamp(const vecpack<N_vecs, vec_N>& v, float lo, float hi) { 
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
vecpack<N_vecs, vec_N> abs(const vecpack<N_vecs, vec_N>& v) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = abs(v[i]);
    return res;
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

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> pow(const vec<N_vecs>& lhs, const vec<vec_N>& rhs) { 
    vecpack<N_vecs, vec_N> res;
    for (auto i = 0; i < vec_N; i++) res[i] = pow(lhs, rhs[i]);
    return res;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> interp(const vec<vec_N>& l, const vec<vec_N>& r, vec<N_vecs> a) {
    vecpack<N_vecs, vec_N> lpack(l);
    vecpack<N_vecs, vec_N> rpack(r);

    return a * lpack + (1.0f - a) * rpack;
}

template<size_t N_vecs, size_t vec_N>
vecpack<N_vecs, vec_N> interp(const vecpack<N_vecs, vec_N>& l, const vecpack<N_vecs, vec_N>& r, vec<N_vecs> a) {
    return a * l + (1.0f - a) * r;
}

template<size_t N_vecs, size_t vec_N>
std::ostream& operator<<(std::ostream& o, const vecpack<N_vecs, vec_N>& v) {
    for (auto i = 0; i < vec_N; i++) {
        o << i << ". " << v[i];
        if (i + 1 < N_vecs) o << std::endl;
    }
    return o;
}

#endif
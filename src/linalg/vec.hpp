#ifndef VEC_HPP
#define VEC_HPP

#define ALIGN __attribute__ ((aligned(32)))

#include <iostream>
#include <cmath>
#include <array>

template<size_t N>
struct vec {
    ALIGN std::array<float, N> data;

    constexpr vec(const std::array<float, N>& odata) {
        std::copy(std::begin(odata), std::end(odata), std::begin(this->data));
    }
    constexpr vec() {
        std::fill(std::begin(this->data), std::end(this->data), 0.0f);
    }
    constexpr vec(float f) {
        std::fill(std::begin(this->data), std::end(this->data), f);
    }
    float& operator[](int i) {
        return this->data[i];
    }
    const float& operator[](int i) const {
        return this->data[i];
    }
};

struct vec2 : vec<2> {
    constexpr vec2() : vec<2>() {}
    constexpr vec2(float f) : vec<2>(f) {}
    constexpr vec2(float x, float y) : vec<2>({x, y}) {}
    constexpr vec2(const vec<2>& v) : vec<2>({v[0], v[1]}) {}
};

struct vec3 : vec<3> {
    constexpr vec3() : vec<3>() {}
    constexpr vec3(float f) : vec<3>(f) {}
    constexpr vec3(float x, float y, float z) : vec<3>({x, y, z}) {}
    constexpr vec3(const vec<3>& v) : vec<3>({v[0], v[1], v[2]}) {}
    constexpr vec3(const vec<2>& xy, float z) : vec<3>({xy[0], xy[1], z}) {}
};


// SIMD Implementation
#include <immintrin.h> 
#include "_mm256_exp_ps.hpp"

#define LOAD(name, vec) const __m256 name = _mm256_load_ps(vec.data.data());
#define LOADC(name, x) \
    ALIGN float xarr[8] = {x, x, x, x, x, x, x, x};\
    const __m256 name = _mm256_load_ps(xarr);

#define STORE(op, res) \
    const __m256 c = op;\
    _mm256_store_ps(res.data.data(), c);

#define MONO_OP(op, v, res) \
    LOAD(a, v);\
    STORE(op(a), res);

#define BI_OP(op, lhs, rhs, res) \
    LOAD(a, lhs);\
    LOAD(b, rhs);\
    STORE(op(a, b), res);


template<size_t N>
struct vec;

vec<8> operator+(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_add_ps, lhs, rhs, res)
    return res;
}

vec<8> operator-(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_sub_ps, lhs, rhs, res)
    return res;
}

vec<8> operator-(const vec<8>& lhs, const float x) { 
    LOAD(a, lhs);
    LOADC(b, x);

    vec<8> res;
    STORE(_mm256_sub_ps(a, b), res)
    return res;
}

vec<8> operator-(const float x, const vec<8>& rhs) { 
    LOADC(a, x);
    LOAD(b, rhs);
    vec<8> res;
    STORE(_mm256_sub_ps(a, b), res);
    return res;
}

vec<8> operator*(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_mul_ps, lhs, rhs, res)
    return res;
}

vec<8> operator/(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_div_ps, lhs, rhs, res)
    return res;
}

float dot(const vec<8>& lhs, const vec<8>& rhs) { 
    LOAD(a, lhs);
    LOAD(b, rhs);
    const __m256 c = _mm256_dp_ps(a, b, 0xff);
    return ((float*)&c)[0];
}

vec<8> sqrt(const vec<8>& v) { 
    vec<8> res;
    MONO_OP(_mm256_sqrt_ps, v, res);
    return res;
}

vec<8> min(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_min_ps, lhs, rhs, res);
    return res;
}

vec<8> max(const vec<8>& lhs, const vec<8>& rhs) { 
    vec<8> res;
    BI_OP(_mm256_max_ps, lhs, rhs, res);
    return res;
}

vec<8> max(const vec<8>& v, float x) { 
    LOAD(a, v);
    LOADC(b, x);
    vec<8> res;
    STORE(_mm256_max_ps(a, b), res);
    return res;
}

vec<8> exp(const vec<8>& v) {
    vec<8> res;
    MONO_OP(_mm256_exp_ps, v, res);
    return res;
}

// General Implementation


template<size_t N>
vec<N> operator-(const vec<N>& v) {
    return -1.0f * v;
}

template<size_t N>
vec<N> operator==(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] == rhs[i];
    return res;
}

template<size_t N>
vec<N> operator==(float lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs == rhs[i];
    return res;
}

template<size_t N>
vec<N> operator==(const vec<N>& lhs, float rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] == rhs;
    return res;
}

template<size_t N>
vec<N> operator<(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] < rhs[i];
    return res;
}

template<size_t N>
vec<N> operator<(float lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs < rhs[i];
    return res;
}

template<size_t N>
vec<N> operator<(const vec<N>& lhs, float rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] < rhs;
    return res;
}

template<size_t N>
vec<N> operator>(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] > rhs[i];
    return res;
}

template<size_t N>
vec<N> operator>(float lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs > rhs[i];
    return res;
}

template<size_t N>
vec<N> operator>(const vec<N>& lhs, float rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] > rhs;
    return res;
}

template<size_t N>
vec<N> operator>=(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] >= rhs[i];
    return res;
}

template<size_t N>
vec<N> operator>=(float lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs >= rhs[i];
    return res;
}

template<size_t N>
vec<N> operator>=(const vec<N>& lhs, float rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] >= rhs;
    return res;
}

template<size_t N>
vec<N> operator<=(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] <= rhs[i];
    return res;
}

template<size_t N>
vec<N> operator<=(float lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs <= rhs[i];
    return res;
}

template<size_t N>
vec<N> operator<=(const vec<N>& lhs, float rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] <= rhs;
    return res;
}

template<size_t N>
vec<N> operator+(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] + rhs[i];
    return res;
}

template<size_t N>
vec<N> operator+(float lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs + rhs[i];
    return res;
}

template<size_t N>
vec<N> operator+(const vec<N>& lhs, float rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] + rhs;
    return res;
}

template<size_t N>
vec<N> operator-(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] - rhs[i];
    return res;
}

template<size_t N>
vec<N> operator-(float lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs - rhs[i];
    return res;
}

template<size_t N>
vec<N> operator-(const vec<N>& lhs, float rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] - rhs;
    return res;
}

template<size_t N>
vec<N> operator*(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] * rhs[i];
    return res;
}

template<size_t N>
vec<N> operator*(float lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs * rhs[i];
    return res;
}

template<size_t N>
vec<N> operator*(const vec<N>& lhs, float rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] * rhs;
    return res;
}

template<size_t N>
vec<N> operator/(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] / rhs[i];
    return res;
}

template<size_t N>
vec<N> operator/(float lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs / rhs[i];
    return res;
}

template<size_t N>
vec<N> operator/(const vec<N>& lhs, float rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] / rhs;
    return res;
}

template<size_t N>
vec<N> apply(const vec<N>& v, float (&func) (float)) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = func(v[i]);
    return res;
}

template<size_t N>
vec<N> max(const vec<N>& v, float x) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::max(v[i], x);
    return res;
}

template<size_t N>
vec<N> max(const vec<N>& v, const vec<N>& w) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::max(v[i], w[i]);
    return res;
}

template<size_t N>
vec<N> min(const vec<N>& v, float x) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::min(v[i], x);
    return res;
}

template<size_t N>
vec<N> min(const vec<N>& v, const vec<N>& w) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::min(v[i], w[i]);
    return res;
}

template<size_t N>
vec<N> clamp(const vec<N>& v, float lo, float hi) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::clamp(v[i], lo, hi);
    return res;
}

template<size_t N>
vec<N> abs(const vec<N>& v) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = abs(v[i]);
    return res;
}

template<size_t N>
float len(const vec<N>& v) {
    return sqrtf(dot(v, v));
}

template<size_t N>
float sum(const vec<N>& v) { 
    float res = 0.0f;
    for (auto i = 0; i < N; i++) res += v[i];
    return res;
}

template<size_t N>
float dot(const vec<N>& lhs, const vec<N>& rhs) { 
    float res = 0.0f;
    for (auto i = 0; i < N; i++) res += lhs[i] * rhs[i];
    return res;
}

template<size_t N>
vec<N> normalize(const vec<N>& v) { 
    float vlen = len(v);
    if (vlen == 0) return v;
    return v/vlen;
}

template<size_t N>
vec<N> sqrt(const vec<N>& v) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = sqrtf(v[i]);
    return res;
}

template<size_t N>
vec<N> interp(const vec<N>& v, const vec<N>& w, float a) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = a * v[i] + (1 - a)*w[i];
    return res;
}

template<size_t N>
vec<N> pow(const vec<N>& v, float a) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = pow(v[i], a);
    return res;
}

template<size_t N>
vec<N> pow(float a, const vec<N>& v) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = pow(a, v[i]);
    return res;
}

template <size_t N>
std::ostream& operator<<(std::ostream& o, const vec<N>& v) {
    copy(v.data.cbegin(), v.data.cend(), std::ostream_iterator<float>(o, " "));
    return o;
}

#endif
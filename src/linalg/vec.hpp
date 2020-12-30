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
    vec<N>& operator=(float x) {
        std::fill(std::begin(this->data), std::end(this->data), x);
        return *this;
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
#include "_mm256_extensions.hpp"

template<>
struct vec<8> {
    __m256 data;
    vec<8>() : vec<8>(0.0f) {}
    vec<8>(__m256 const& x) : data(x) {}
    vec<8>(float x) {
        ALIGN float xarr[8] = {x, x, x, x, x, x, x, x};
        data = _mm256_load_ps(xarr);
    }
    vec<8>(std::array<float, 8> x) {
        ALIGN float xarr[8];
        std::copy(std::begin(x), std::end(x), std::begin(xarr));
        data = _mm256_load_ps(xarr);
    }

    operator __m256() const {
        return data;
    }

    operator std::array<float, 8>() const {
        std::array<float, 8> res;
        _mm256_store_ps(res.data(), data);
        return res;
    }

    vec<8>& operator=(__m256 const& x) {
        data = x;
        return *this;
    }

    vec<8>& operator=(float x) {
        data = _mm256_set1_ps(x);
        return *this;
    }

    vec<8>& operator=(std::array<float, 8> x) {
        ALIGN float xarr[8];
        std::copy(std::begin(x), std::end(x), std::begin(xarr));
        data = _mm256_load_ps(xarr);
        return *this;
    }
};

vec<8> operator+(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_add_ps(lhs, rhs);
}

vec<8> operator-(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_sub_ps(lhs, rhs);
}

vec<8> operator*(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_mul_ps(lhs, rhs);
}

vec<8> operator/(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_div_ps(lhs, rhs);
}

vec<8> operator%(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_mod_ps(lhs, rhs);
}

vec<8> min(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_min_ps(lhs, rhs);
}

vec<8> min(const vec<8>& v, float x) { 
    return _mm256_min_ps(v, vec<8>(x));
}

vec<8> operator==(const vec<8>& lhs, const vec<8>& rhs) {
    return min(_mm256_cmp_ps(lhs, rhs, 0), 1.0f);
}

vec<8> operator<(const vec<8>& lhs, const vec<8>& rhs) { 
    return min(_mm256_cmp_ps(lhs, rhs, 1), 1.0f);
}

vec<8> operator<=(const vec<8>& lhs, const vec<8>& rhs) { 
    return min(_mm256_cmp_ps(lhs, rhs, 2), 1.0f);
}

vec<8> operator>(const vec<8>& lhs, const vec<8>& rhs) { 
    return rhs < lhs;
}

vec<8> operator>=(const vec<8>& lhs, const vec<8>& rhs) { 
    return rhs <= lhs;
}

float dot(const vec<8>& lhs, const vec<8>& rhs) { 
    const __m256 c = _mm256_dp_ps(lhs, rhs, 0xff);
    return ((float*)&c)[0];
}

vec<8> sqrt(const vec<8>& v) { 
    return _mm256_sqrt_ps(v);
}

vec<8> max(const vec<8>& lhs, const vec<8>& rhs) { 
    return _mm256_max_ps(lhs, rhs);
}

vec<8> max(const vec<8>& lhs, float rhs) { 
    return _mm256_max_ps(lhs, vec<8>(rhs));
}

vec<8> exp(const vec<8>& v) {
    return _mm256_exp_ps(v);
}

vec<8> abs(const vec<8>& v) {
    return _mm256_abs_ps(v);
}

vec<8> pow(const vec<8>& lhs, const vec<8>& rhs) {
    return _mm256_pow_ps(lhs, rhs);
}

vec<8> clamp(const vec<8>& v, float lo, float hi) {
    return max(min(v, hi), lo);
}

float sum(const vec<8>& v) { 
    return dot(v, 1.0f);
}

std::ostream& operator<<(std::ostream& o, const vec<8>& v) {
    std::array<float, 8> data = v;
    copy(data.cbegin(), data.cend(), std::ostream_iterator<float>(o, " "));
    return o;
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
vec<N> operator==(float lhs, const vec<N>& rhs) { return vec<N>(lhs) == rhs; }

template<size_t N>
vec<N> operator==(const vec<N>& lhs, float rhs) { return rhs == lhs; }

template<size_t N>
vec<N> operator<(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] < rhs[i];
    return res;
}

template<size_t N>
vec<N> operator<(float lhs, const vec<N>& rhs) { return vec<N>(lhs) < rhs; }

template<size_t N>
vec<N> operator<(const vec<N>& lhs, float rhs) { return lhs < vec<N>(rhs); }

template<size_t N>
vec<N> operator>(const vec<N>& lhs, const vec<N>& rhs) { return rhs < lhs; }

template<size_t N>
vec<N> operator>(float lhs, const vec<N>& rhs) { return vec<N>(lhs) > rhs; }

template<size_t N>
vec<N> operator>(const vec<N>& lhs, float rhs) { return lhs > vec<N>(rhs); }

template<size_t N>
vec<N> operator>=(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] >= rhs[i];
    return res;
}

template<size_t N>
vec<N> operator>=(float lhs, const vec<N>& rhs) { return vec<N>(lhs) >= rhs; }

template<size_t N>
vec<N> operator>=(const vec<N>& lhs, float rhs) { return lhs >= vec<N>(rhs); }

template<size_t N>
vec<N> operator<=(const vec<N>& lhs, const vec<N>& rhs) { return rhs >= lhs; }

template<size_t N>
vec<N> operator<=(float lhs, const vec<N>& rhs) { return rhs >= lhs; }

template<size_t N>
vec<N> operator<=(const vec<N>& lhs, float rhs) { return rhs >= lhs; }

template<size_t N>
vec<N> operator+(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] + rhs[i];
    return res;
}

template<size_t N>
vec<N> operator+(float lhs, const vec<N>& rhs) { return vec<N>(lhs) + rhs; }

template<size_t N>
vec<N> operator+(const vec<N>& lhs, float rhs) { return rhs + lhs; }

template<size_t N>
vec<N> operator-(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] - rhs[i];
    return res;
}

template<size_t N>
vec<N> operator-(float lhs, const vec<N>& rhs) { return vec<N>(lhs) - rhs; }

template<size_t N>
vec<N> operator-(const vec<N>& lhs, float rhs) { return lhs - vec<N>(rhs); }

template<size_t N>
vec<N> operator*(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] * rhs[i];
    return res;
}

template<size_t N>
vec<N> operator*(float lhs, const vec<N>& rhs) { return vec<N>(lhs) * rhs; }

template<size_t N>
vec<N> operator*(const vec<N>& lhs, float rhs) { return rhs * lhs; }


template<size_t N>
vec<N> operator/(const vec<N>& lhs, const vec<N>& rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = lhs[i] / rhs[i];
    return res;
}

template<size_t N>
vec<N> operator/(float lhs, const vec<N>& rhs) { return vec<N>(lhs) / rhs; }

template<size_t N>
vec<N> operator/(const vec<N>& lhs, float rhs) { return lhs / vec<N>(rhs); }

template<size_t N>
vec<N> operator%(const vec<N>& lhs, const vec<N>& rhs) {
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = fmodf(lhs[i], rhs[i]);
    return res;
}

template<size_t N>
vec<N> max(const vec<N>& v, const vec<N>& w) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::max(v[i], w[i]);
    return res;
}

template<size_t N>
vec<N> max(const vec<N>& lhs, float rhs) { 
    return max(lhs, vec<N>(rhs));
}

template<size_t N>
vec<N> min(const vec<N>& v, const vec<N>& w) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = std::min(v[i], w[i]);
    return res;
}

template<size_t N>
vec<N> min(const vec<N>& lhs, float rhs) { 
    return min(lhs, vec<N>(rhs));
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
vec<N> pow(const vec<N>& lhs, vec<N> rhs) { 
    vec<N> res;
    for (auto i = 0; i < N; i++) res[i] = pow(lhs[i], rhs[i]);
    return res;
}

template<size_t N>
vec<N> pow(const vec<N>& v, float a) { 
    return pow(v, vec<N>(a));
}

template<size_t N>
vec<N> pow(float a, const vec<N>& v) { 
    return pow(vec<N>(a), v);
}

template <size_t N>
std::ostream& operator<<(std::ostream& o, const vec<N>& v) {
    copy(v.data.cbegin(), v.data.cend(), std::ostream_iterator<float>(o, " "));
    return o;
}

#endif
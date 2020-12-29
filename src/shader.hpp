#ifndef SHADER_HPP
#define SHADER_HPP

#include <algorithm>

#include "types.hpp"
#include "camera.hpp"
#include "linalg/vec.hpp"
#include "scenes/scene.hpp"
#include "transformations.hpp"
#include "distances.hpp"

struct ShaderConfig {
    // this shouldn't really change
    float max_dist;
    int max_its;

    vec3 light_dir;
    vec3 background_color;

    // this will change
    float time;
};

class Shader {
    public:
    Shader(const ShaderConfig* config, const Camera* camera, const Scene* scene) : config(config), camera(camera), scene(scene) {}
    color render_pixel(const size_t x, const size_t y) const;
    std::array<color, 8> render_pixel_simd(const vecpack<8, 2>& pixels) const;

    private:
    vec2 march(const float t, const vec3& direction) const;
    vecpack<8, 2> march_simd(const float t, const vecpack<8, 3>& directions) const;

    vec3 normal(const float t, const vec3& p) const;
    vecpack<8, 3> normal_simd(const float t, const vecpack<8, 3>& p) const;

    float ambient(const vec3& p, const vec3& n) const;
    vec<8> ambient_simd(const vecpack<8, 3>& p, const vecpack<8, 3>& n) const;

    float shadow(const float t, const vec3& p, int k) const;
    vec<8> shadow_simd(const float t, const vecpack<8, 3>& p, int k) const;

    vec3 apply_fog(const vec3& original_color, float distance, const vec3& ray_dir, const vec3& sun_dir) const;
    vecpack<8, 3> apply_fog_simd(const vecpack<8, 3>& original_color, vec<8> distance, const vecpack<8, 3>& ray_dir, const vecpack<8, 3>& sun_dir) const;

    const ShaderConfig* config;
    const Camera* camera;
    const Scene* scene;
};

std::array<color, 8> Shader::render_pixel_simd(const vecpack<8, 2>& pixels) const {
    vecpack<8, 3> dir = camera->get_ray_dir_simd(pixels);
    std::array<color, 8> colors;

    vecpack<8, 2> res = march_simd(config->time, dir);
    vec<8> hit_time = res[0];
    vec<8> hit_texture = res[1];
    vec<8> col_mask = hit_time >= 0;
    
    vecpack<8, 3> fcolors = scene->texture_simd(hit_time, hit_texture);

    vecpack<8, 3> p = camera->position + hit_time * dir;
    vecpack<8, 3> n = normal_simd(config->time, p);

    vec<8> sun = ambient_simd(p, n);
    vec<8> sha = shadow_simd(config->time, p + 0.1f * n, 32);
    vec<8> sky = clamp(0.5f + 0.5f*n[1], 0.0f, 1.0f);

    vec<3> l = normalize(config->light_dir * vec3(-1.0,0.0,-1.0));
    vec<8> ind = clamp(dot(n, l), 0.0f, 1.0f);

    vecpack<8, 3> lin(vec3(0.0f, 0.0f, 0.0f));
    lin = lin + sun * vec3(1.64,1.27,0.99)/2.0f * pow(sha,vec3(1.0,1.2,1.5));
    lin = lin + sky * vec3(0.16,0.20,0.28);
    lin = lin + ind * vec3(0.40,0.28,0.20);

    fcolors = lin * fcolors;
    fcolors = col_mask * fcolors + (1.0f - col_mask) * config->background_color;

    fcolors = apply_fog_simd(fcolors, hit_time, dir, config->light_dir);

    fcolors = 255.0f * fcolors;

    std::array<float, 8> r = fcolors[0],
                         g = fcolors[1],
                         b = fcolors[2];

    for (auto i = 0; i < 8; i++) {
        colors[i] = std::make_tuple(
            (unsigned char)(int)r[i],
            (unsigned char)(int)g[i],
            (unsigned char)(int)b[i]
        );
    }
    return colors;
}

color Shader::render_pixel(const size_t x, const size_t y) const {
    vec3 dir = camera->get_ray_dir(vec2(x, y));
    vec3 color;
    vec2 res = march(config->time, dir);

    float time = config->time;

    float hit_time = res[0];
    float hit_texture = res[1];

    if (hit_time < 0) {
        color = config->background_color;
    } else {
        vec3 p = camera->position + hit_time * dir;
        vec3 n = normal(config->time, p);

        float sha = shadow(config->time, p, 32);
        float sun = std::clamp(dot(n, config->light_dir), 0.0f, 1.0f);
        float sky = std::clamp(0.5f + 0.5f*n[1], 0.0f, 1.0f);
        float ind = std::clamp(dot(n, normalize(config->light_dir*vec3(-1.0,0.0,-1.0))), 0.0f, 1.0f);

        // compute lighting
        vec3 lin = vec3(0,0,0);
        lin = lin + sun * vec3(1.64,1.27,0.99)/2 * pow(sha,vec3(1.0,1.2,1.5));
        lin = lin + sky * vec3(0.16,0.20,0.28);
        lin = lin + ind * vec3(0.40,0.28,0.20);

        color = lin * scene->texture((int)hit_texture, p);
    }

    color = apply_fog(color, hit_time, dir, config->light_dir);

    color = 255.0f * color;


    return std::make_tuple(
        (unsigned char)(int)color[0],
        (unsigned char)(int)color[1],
        (unsigned char)(int)color[2]
    );
}

vec3 Shader::normal(const float t, const vec3& p) const {
    float d = 0.5773*0.0001;
    vec3 d1(d,-d,-d);
    vec3 d2(-d,-d,d);
    vec3 d3(-d,d,-d);
    vec3 d4(d,d,d);

    return normalize(d1*scene->dist_field(t, p+d1)[0] + d2*scene->dist_field(t, p+d2)[0] + 
                     d3*scene->dist_field(t, p+d3)[0] + d4*scene->dist_field(t, p+d4)[0]);
}

vecpack<8, 3> Shader::normal_simd(const float t, const vecpack<8, 3>& p) const {
    float d = 0.5773*0.0001;
    vecpack<8, 3> d1 = vecpack3<8>(d,-d,-d);
    vecpack<8, 3> d2 = vecpack3<8>(-d,-d,d);
    vecpack<8, 3> d3 = vecpack3<8>(-d,d,-d);
    vecpack<8, 3> d4 = vecpack3<8>(d,d,d);

    return normalize(d1*scene->dist_field_simd(t, p+d1)[0] + d2*scene->dist_field_simd(t, p+d2)[0] + 
                     d3*scene->dist_field_simd(t, p+d3)[0] + d4*scene->dist_field_simd(t, p+d4)[0]);
}

float Shader::ambient(const vec3& p, const vec3& n) const {
    return std::clamp(dot(n, config->light_dir), 0.0f, 1.0f);
}

vec<8> Shader::ambient_simd(const vecpack<8, 3>& p, const vecpack<8, 3>& n) const {
    vec<8> dots = dot(n, config->light_dir);
    return clamp(dots, 0.0f, 1.0f);
}

vec2 Shader::march(const float gt, const vec3& direction) const {
    vec2 res(config->max_dist, 0);
    float t = 1.0;

    for (int s = 0; s < config->max_its && t < config->max_dist; s++) {
        res = scene->dist_field(gt, camera->position + t * direction);
        if (res[0] < 0.0005*t) {
            return vec2(t, res[1]);
        }
        t += res[0];
    }

    return vec2(-1, 0);
}

vecpack<8, 2> Shader::march_simd(const float gt, const vecpack<8, 3>& directions) const {
    vecpack<8, 2> res;
    vec<8> distance, texture, t(1.0f), collided(0.0f), col_mask(0.0f);

    for (int s = 0; s < config->max_its; s++) {
        vecpack<8, 2> res = scene->dist_field_simd(gt,
            camera->position + t * directions
        );
        distance = res[0];
        texture = res[1];

        collided = distance < 0.0005 * t;
        col_mask = max(col_mask, collided);

        t = t + distance * (1.0 - col_mask);

        if (sum(col_mask) == 8) break;
    }

    // t = -1 if no collision, else distance
    t = col_mask * t - (1.0f - col_mask);
    texture = col_mask * texture;

    return vecpack<8, 2>({t, texture});
}

float Shader::shadow(const float gt, const vec3& p, int k) const {
    float t = 0.01;
    float h;
    float res = 1.0;
    vec2 dres;

    for (int s = 0; s < 16 || t < 6.0; s++) {
        dres = scene->dist_field(gt, p + t*config->light_dir);
        h = dres[0];
        res = std::min(res, k*std::max(0.0f, h)/t);
        if (h < 0.0001) {
            res = 0.0;
            break;
        }
        
        t += std::clamp(h, 0.01f, 0.5f);
    }
    
    return res;
}

vec<8> Shader::shadow_simd(const float gt, const vecpack<8, 3>& p, int k) const {
    vecpack<8, 3> dir(config->light_dir);
    vec<8> distance, texture, t(1.0f), collided(0.0f), col_mask(0.0f), res(1.0f);


    for (int s = 0; s < 16; s++) {
        vecpack<8, 2> dres = scene->dist_field_simd(gt, p + t * dir);
        distance = dres[0];

        res = min(res, k*max(0.0f, distance)/t);

        collided = distance < 0.0001;
        col_mask = max(col_mask, collided);

        t = t + (1.0 - col_mask) * clamp(distance, 0.01f, 0.5f);

        if (sum(col_mask) == 8) break;
    }

    // res = 0.0f if collision, else res
    return (1.0f - col_mask) * res;
}

vec3 Shader::apply_fog(const vec3& original_color, float distance, const vec3& ray_dir, const vec3& sun_dir) const {
    float fog = 1.0 - exp(-powf(distance/40.0f, 2));
    float sun = std::max(dot(ray_dir, sun_dir), 0.0f);
    vec3 fog_color = interp(
        vec3(1.0,0.9,0.7), // sun yellow-ish
        vec3(0.5, 0.6, 0.7), // sky blue-ish
        sun
    );
    return interp(fog_color, original_color, fog);
}

vecpack<8, 3> Shader::apply_fog_simd(const vecpack<8, 3>& original_color, vec<8> distance, const vecpack<8, 3>& ray_dir, const vecpack<8, 3>& sun_dir) const {
    vec<8> scaled_dist = distance/40.0f;
    vec<8> fog = 1.0 - exp(-scaled_dist*scaled_dist);

    vec<8> sun = max(dot(ray_dir, sun_dir), 0.0f);
    vecpack<8, 3> fog_color = interp(
        vec3(1.0,0.9,0.7), // sun yellow-ish
        vec3(0.5, 0.6, 0.7), // sky blue-ish
        sun
    );

    return interp(fog_color, original_color, fog);
}



#endif
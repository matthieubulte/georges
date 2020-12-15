#ifndef SHADER_HPP
#define SHADER_HPP

#include <algorithm>

#include "types.hpp"
#include "camera.hpp"
#include "vec.hpp"

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
    Shader(const ShaderConfig* config, const Camera* camera) : config(config), camera(camera) {}
    color render_pixel(const size_t x, const size_t y) const;

    private:
    vec2 dist_field(const vec3& p) const;
    vec3 normal(const vec3& p) const;
    float ambient(const vec3& p, const vec3& n) const;
    vec3 texture(int texture_id, const vec3& pos) const;
    vec2 march(const vec3& direction) const;
    float shadow(const vec3& p, const vec3& n, int k) const;
    vec3 apply_fog(const vec3& original_color, float distance, const vec3& ray_dir, const vec3& sun_dir) const;

    const ShaderConfig* config;
    const Camera* camera;
};


color Shader::render_pixel(const size_t x, const size_t y) const {
    vec3 dir = camera->get_ray_dir(vec2(x, y));
    vec3 color;
    vec2 res = march(dir);

    float time = config->time;

    float hit_time = res[0];
    float hit_texture = res[1];

    if (hit_time < 0) {
        color = config->background_color;
    } else {
        vec3 p = camera->position + hit_time * dir;
        vec3 n = normal(p);

        float sha = std::clamp(shadow(p+n*0.1, n, 32), 0.0f, 1.0f);
        float sun = std::clamp(dot(n, config->light_dir), 0.0f, 1.0f);
        float sky = std::clamp(0.5f + 0.5f*n[1], 0.0f, 1.0f);
        float ind = std::clamp(dot(n, normalize(config->light_dir*vec3(-1.0,0.0,-1.0))), 0.0f, 1.0f);

        // compute lighting
        vec3 lin = vec3(0,0,0);
        lin = lin + sun * vec3(1.64,1.27,0.99)/2 * pow(sha,vec3(1.0,1.2,1.5));
        lin = lin + sky * vec3(0.16,0.20,0.28);
        lin = lin + ind * vec3(0.40,0.28,0.20);

        color = lin * texture((int)hit_texture, p);    
    }

    color = apply_fog(color, hit_time, dir, config->light_dir);
    color = pow(color, 1.0/2.2); // gamma correction

    color = 255.0f * color;


    return std::make_tuple(
        (unsigned char)(int)color[0],
        (unsigned char)(int)color[1],
        (unsigned char)(int)color[2]
    );
}

vec2 Shader::dist_field(const vec3& p) const {
    vec3 pt, q;
    float d = config->max_dist;
    
    // floor
    d = dist_plane(vec3(0,1,0), 0, p);

    // sphere
    float s = fmod(config->time/2, 2);
    
    pt = symX(p);
    vec3 vp = vec3(fmod(abs(p[0]),1.5)-.75,p[1],fmod(p[2]+.75,1.5)-.75);
    vec2 id = vec2(floor(p[0]/1.5), floor(p[2]/1.5 + .5));

    float fid = id[0]*11.1 + id[1]*31.7;
    float fy = fmod(fid*1.312+config->time*0.05, 1.0f);
    float y = 4.0*fy - 1.0f;
    float rad = fy*(1.0-fy);

    float d2 = dist_sphere(rad, vp-vec3(0.5,y,0.0));
    d2 -= 0.03*(sin(18.0*p[0])+sin(18.0*p[1])+sin(18.0*p[2]));
    d2 *= 0.6;
    d2 = fmin(d2,2.0);
    
    float ds = smin(d, d2, 0.32);
    return vec2(smin(d, ds, 0.5), /* texture */ 2);
}

vec3 Shader::normal(const vec3& p) const {
    float d = 0.5773*0.0001;
    vec3 d1(d,-d,-d);
    vec3 d2(-d,-d,d);
    vec3 d3(-d,d,-d);
    vec3 d4(d,d,d);

    return normalize(d1*dist_field(p+d1)[0] + d2*dist_field(p+d2)[0] + 
                     d3*dist_field(p+d3)[0] + d4*dist_field(p+d4)[0]);
}

float Shader::ambient(const vec3& p, const vec3& n) const {
    return std::clamp(dot(n, config->light_dir), 0.0f, 1.0f);
}

vec3 Shader::texture(int texture_id, const vec3& pos) const {
    if (texture_id == 1) { // floor
        float x = pos[0] >= 0 ? pos[0] : -pos[0] + 0.5;
        float z = pos[2] >= 0 ? pos[2] : -pos[2] + 0.5;
        return (fmod(x, 1) < 0.5) == (fmod(z, 1) < 0.5) ?
vec3(1,1,1) : vec3(0,0,0);
    } else if (texture_id == 2) { // block
        return vec3(51, 255, 189)/255.0f/5.0;
    } else if (texture_id == 3) { // sphere
        return vec3(255, 189, 51)/255.0f/2.0;
    }
    return vec3(0,1,0);
}

vec2 Shader::march(const vec3& direction) const {
    vec2 res(config->max_dist, 0);
    float t = 1.0;

    for (int s = 0; s < config->max_its && t < config->max_dist; s++) {
        res = dist_field(camera->position + t * direction);
        if (res[0] < 0.0005*t) {
            return vec2(t, res[1]);
        }
        t += res[0];
    }

    return vec2(-1, 0);
}

float Shader::shadow(const vec3& p, const vec3& n, int k) const {
    float t = 0.01;
    float h;
    float res = 1.0;
    vec2 dres;

    for (int s = 0; s < 16 || t < 6.0; s++) {
        dres = dist_field(p + t*config->light_dir);
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



#endif
#include <cmath>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <array>
#include <thread>
#include <vector>

#include "distances.hpp"
#include "transformations.hpp"
#include "mat3.hpp"
#include "vec.hpp"

#include "Screen.hpp"
#include "Controls.hpp"
#include "PerformanceMonitor.hpp"

typedef std::tuple<unsigned char,unsigned char,unsigned char> color;

#define DEG_TO_RAD (M_PI / 180)

const float grad_step = 0.01;
const float max_dist = 10000.0f;
const int max_its = 256;

const vec3 light_dir = normalize(vec3(-0.2, 0.2, 0));
vec3 background_color(0.4,0.56,0.97);

template<size_t N_vecs>
vecpack<N_vecs, 3> ray_dir(float fov, const mat3& view_rot, const vec2& size, const vecpack<N_vecs, 2>& pixels) {
    vecpack<N_vecs, 2> xy = pixels - size * 0.5;

	float cot_half_fov = tan((90.0 - fov * 0.5) * DEG_TO_RAD);
	float z = size[1] * 0.5 * cot_half_fov;
	
    vecpack<N_vecs, 3> dir;
    dir[0] = xy[0];
    dir[1] = xy[1];
    dir[2] = -z;
	
    return view_rot * dir;
}

template<size_t N_vecs>
vecpack<N_vecs, 3> normal(const vecpack<N_vecs, 3>& p) {
    float d = 0.5773*0.0001;
    vecpack<N_vecs, 3> d1 = vecpack3<N_vecs>(d,-d,-d);
    vecpack<N_vecs, 3> d2 = vecpack3<N_vecs>(-d,-d,d);
    vecpack<N_vecs, 3> d3 = vecpack3<N_vecs>(-d,d,-d);
    vecpack<N_vecs, 3> d4 = vecpack3<N_vecs>(d,d,d);

    return normalize(d1*dist_field(p+d1)[0]
                   + d2*dist_field(p+d2)[0]
                   + d3*dist_field(p+d3)[0]
                   + d4*dist_field(p+d4)[0]
            );
}

float clamp(float x, float lo, float hi) {
    return std::min(hi, std::max(lo, x));
}

template<size_t N_vecs>
vecpack<N_vecs, 2> dist_field(const vecpack<N_vecs, 3>& p) {
    vecpack<N_vecs, 3> pt, q;
    vec<N_vecs> d = max_dist;
    
    // floor
    d = dist_plane(vec3(0,1,0), 0, p);

    q = p - vec3(0.0f, 1.0f, 0.0f);
    vec<N_vecs> d2 = dist_sphere(0.5f, q);

    // sphere
    vec<N_vecs> ds = smin(d, d2, 0.32);

    vecpack<N_vecs, 2> res;
    res[0] = ds;
    res[1] = 1.0;
    return res;
}

template<size_t N_vecs>
float ambient(const vecpack<N_vecs, 3>& p, const vecpack<N_vecs, 3>& n) {
    return clamp(dot(n, light_dir), 0, 1);
}

template<size_t N_vecs>
vecpack<N_vecs, 3> texture(vec<N_vecs> texture_ids, const vecpack<N_vecs, 3>& pos) {
//     if (texture_id == 1) { // floor
//         float x = pos[0] >= 0 ? pos[0] : -pos[0] + 0.5;
//         float z = pos[2] >= 0 ? pos[2] : -pos[2] + 0.5;
//         return (fmod(x, 1) < 0.5) == (fmod(z, 1) < 0.5) ?
// vec3(1,1,1) : vec3(0,0,0);
//     } else if (texture_id == 2) { // block
//         return vec3(51, 255, 189)/255.0f/5.0;
//     } else if (texture_id == 3) { // sphere
//         return vec3(255, 189, 51)/255.0f/2.0;
//     }

    return vecpack3<N_vecs>(0.0f,1.0f,0.0f)
         + texture_ids * vecpack3<N_vecs>(51.0f/255.0f/5.0f, 255.0f/255.0f/5.0f, 189.0f/255.0f/5.0f);
}

template<size_t N_vecs>
vecpack<N_vecs, 2> march(const vec3& origin, const vecpack<N_vecs, 3>& direction, int steps) {
    vecpack<N_vecs, 2> res = vecpack2<N_vecs>((float)max_dist, 0.0f);
    vec<N_vecs> distances;
    vec<N_vecs> textures;

    vec<N_vecs> done; done = 0.0f;
    vec<N_vecs> t; t = 1.0f;

    // for (int s = 0; s < steps && t < max_dist; s++) {
    for (int s = 0; s < steps; s++) {
        res = dist_field(origin + t * direction);
        distances = res[0];
        textures = res[1];
        
        done = done * lt(distances, 0.0005*t);
        if (res[0] < 0.0005*t) {
            return vec2(t, res[1]);
        }
        t += res[0];
    }

    return vec2(-1, 0);
}

template<size_t N_vecs>
float shadow(const vecpack<N_vecs, 3>& p, const vecpack<N_vecs, 3>& n, int k) {
    float t = 0.01;
    float h;
    float res = 1.0;
    vec2 dres;

    for (int s = 0; s < 16 || t < 6.0; s++) {
        dres = dist_field(time, p + t*light_dir);
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

template<size_t N_vecs>
vec3 apply_fog(const vec3& original_color, float distance, const vec3& ray_dir, const vec3& sun_dir) {
    float fog = 1.0 - exp(-powf(distance/40.0f, 2));
    float sun = std::max(dot(ray_dir, sun_dir), 0.0f);
    vec3 fog_color = interp(
        vec3(1.0,0.9,0.7), // sun yellow-ish
        vec3(0.5, 0.6, 0.7), // sky blue-ish
        sun
    );
    return interp(fog_color, original_color, fog);
}

template<size_t N_vecs>
std::array<color, N_vecs> renderPixels(const vec3& camera_pos, const mat3& view_rot, const vec2& dim, const vecpack<N_vecs, 2>& pixels) {
    vecpack<N_vecs, 3> dir = ray_dir(45.0, view_rot, dim, pixels);

    vecpack<N_vecs, 3> color;
    vecpack<N_vecs, 2> res = march<N_vecs>(camera_pos, dir, max_its);

    vec<N_vecs> hit_time = res[0];
    vec<N_vecs> hit_texture = res[1];

    if (hit_time < 0) {
        color = background_color;
    } else {
        vecpack<N_vecs, 3> p = camera_pos + hit_time * dir;
        vecpack<N_vecs, 3> n = normal(p);

        vec<N_vecs> sha = std::clamp(shadow(p+n*0.1, n, 32), 0, 1);
        vec<N_vecs> sun = clamp(dot(n, light_dir), 0, 1);
        vec<N_vecs> sky = clamp(0.5 + 0.5*n[1], 0, 1);
        vec<N_vecs> ind = clamp(dot(n, normalize(light_dir*vec3(-1.0,0.0,-1.0))), 0, 1);

        // compute lighting
        vec3 lin = vec3(0,0,0);
        lin = lin + sun * vec3(1.64,1.27,0.99)/2 * pow(sha,vec3(1.0,1.2,1.5));
        lin = lin + sky * vec3(0.16,0.20,0.28);
        lin = lin + ind * vec3(0.40,0.28,0.20);

        color = lin * texture(time, (int)hit_texture, p);    
    }

    color = apply_fog(color, hit_time, dir, light_dir);
    color = pow(color, 1.0/2.2); // gamma correction

    color = 255.0f * color;


    return std::make_tuple(
        (unsigned char)(int)color[0],
        (unsigned char)(int)color[1],
        (unsigned char)(int)color[2]
    );
}

template<size_t dimx, size_t dimy, size_t min_offset, size_t max_offset, size_t N_vecs>
void splash_color(Screen<dimx, dimy>* screen, const vecpack<N_vecs, 2>& pixels_to_render, const std::array<color, N_vecs>& c) {
    for (auto i = 0; i < N_vecs; i++) {
        const size_t x = pixels_to_render[0][i];
        const size_t y = pixels_to_render[1][i];

        const unsigned char r = std::get<0>(c[i]), g = std::get<1>(c[i]), b = std::get<2>(c[i]);

        screen->put_pixel(x, y, r, g, b);
    }
}

template<size_t dimx, size_t dimy, size_t min_offset, size_t max_offset, size_t N_vecs>
void paint(Screen<dimx, dimy>* screen, const vec3* camera_pos, const mat3* camera_rot) {
    constexpr size_t range = max_offset - min_offset;
    const vec2 dim(dimx, dimy);

    vecpack<N_vecs, 2> pixels_to_render;

    for (auto i = 0; i < 10000; i++) {

        for (auto j = 0; j < N_vecs; j++) {
            const size_t local_offset = rand() % range;
            const size_t offset = min_offset + local_offset;

            const unsigned int x = offset % dimx;
            pixels_to_render[0][j] = x; // x[j]
            pixels_to_render[1][j] = dimy - 1 - (offset - x) / dimx; //y[j]
        }

        std::array<color, N_vecs> c = renderPixels<N_vecs>(*camera_pos, *camera_rot, dim, pixels_to_render);

        splash_color<dimx, dimy, min_offset, max_offset, N_vecs>(screen, pixels_to_render, c);
    }
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    constexpr vec2 dim(dimx, dimy);

    const size_t seconds_between_logs = 1;
    const float walk_speed = 0.2f;
    const float turn_speed = 0.05f;
    
    unsigned long frame_number = 0;
    vec3 camera_pos = vec3(0.0, 1.0, 0.0);
    float view_rot = M_PI;
    mat3 rot = rotationY(view_rot);
    float time = 0;
    
    Screen<dimx, dimy> screen;
    PerformanceMonitor perf(2);
    controles_state state;
    
    if (!screen.initialize("")) return -1;
    
    constexpr size_t num_threads = 8;
    constexpr size_t pixels_per_thread = dimx * dimy / num_threads;

    while(!state.quit) {
        poll_state(state);

        view_rot += (state.left - state.right) * turn_speed;
        view_rot = fmodf(view_rot, 2.0 * M_PI);

        rot = rotationY(view_rot);

        vec3 walk_dir = vec3(0, 0, (state.down - state.up) * walk_speed);
        camera_pos = camera_pos + rot * walk_dir;

        paint<dimx, dimy, 0, dimx*dimy, 4>(&screen, &camera_pos, &rot);

        screen.render();
        perf.tick();
        // screen.sleep(100);
        frame_number++;
        time += 100.f / 1000.0f;
    }

    return EXIT_SUCCESS;
}

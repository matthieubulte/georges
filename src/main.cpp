#include <cmath>
#include <tuple>
#include <iostream>
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

#define DEF_RENDER_THREAD(i) std::thread t##i(\
    painter_thread<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread>,\
     i, &screen, &camera_pos, &rot, &state.quit, &frame_number, &time);

using namespace std;

typedef std::tuple<unsigned char,unsigned char,unsigned char> color;

#define DEG_TO_RAD (M_PI / 180)

const float grad_step = 0.01;
const float max_dist = 10000.0f;
const int max_its = 256;

const vec3 light_dir = normalize(vec3(-0.2, 0.2, 0));
vec3 background_color(0.4,0.56,0.97);

vec3 ray_dir(float fov, const mat3& view_rot, const vec2& size, const vec2& pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan((90.0 - fov * 0.5) * DEG_TO_RAD);
	float z = size[1] * 0.5 * cot_half_fov;
	
	vec3 dir = normalize(vec3(xy, -z));
    return view_rot * dir;
}

vec2 dist_field(const float t, const vec3& p) {
    vec3 pt, q;
    float d = max_dist;
    
    // floor
    // pt = translate(vec3(0, 0.075*sin(p[0])*sin(p[2]), 0), p);
    d = dist_plane(vec3(0,1,0), 0, p);

    // sphere
    float s = fmod(t/2, 2);
    
    pt = symX(p);
    vec3 vp = vec3(fmod(abs(p[0]),1.5)-.75,p[1],fmod(p[2]+.75,1.5)-.75);
    vec2 id = vec2(floor(p[0]/1.5), floor(p[2]/1.5 + .5));

    float fid = id[0]*11.1 + id[1]*31.7;
    float fy = fmod(fid*1.312+t*0.05, 1.0f);
    float y = 4.0*fy - 1.0f;
    float rad = fy*(1.0-fy);

    float d2 = dist_sphere(rad, vp-vec3(0.5,y,0.0));
    d2 -= 0.03*(sin(18.0*p[0])+sin(18.0*p[1])+sin(18.0*p[2]));
    d2 *= 0.6;
    d2 = fmin(d2,2.0);
    
    float ds = smin(d, d2, 0.32);


    vec2 rs = vec2(smin(d, ds, 0.5), /* texture */ 2);
    // pt = translate(bpos, p);
    
    
    // float rad = 0.1 + (1 - clamp(s, 0.0f, 1.0f)) / 4;
    // float ds = dist_sphere(rad, pt);
    
    // vec2 rs = vec2(smin(d, ds, 0.5), /* texture */ 2);

    // column
    // pt = translate(vec3(0,.75,-1.), p);
    // pt = repeatX(2, pt);
    // d = dist_box(vec3(1, 0.2, 1), pt);

    // float crad = 0.15*(1 + 0.1*sin(atan2f(pt[0], pt[2])*20.0f));
    // crad -= 0.05*pt[1];
    // crad += 0.1*pow(0.1 + 0.1*sin(pt[1]*20), 0.01);

    // d = len(vec2(pt[0], pt[2])) - crad;
    // d = max(d, pt[1]-1.1f);
    // d = max(d, -pt[1]-5.0f);

    // vec2 rb = vec2(d, /* texture */ 3);

    // boxes
    // q = translate(vec3(0, 1.1, 0), pt);
    // d = dist_box(vec3(0.35, 0.05, 0.35), q);
    // vec2 rbb = vec2(d, /* texture */ 3);

    // q = translate(vec3(0, -0.7, 0), pt);
    // d = dist_box(vec3(0.35, 0.05, 0.35), q);
    // rbb = dist_union(rbb, vec2(d, /* texture */ 3));
    
    return rs; //dist_union(rp, rb, dist_union(rbb, rs));
}

vec3 normal(const float t, const vec3& p) {
    float d = 0.5773*0.0001;
    vec3 d1(d,-d,-d);
    vec3 d2(-d,-d,d);
    vec3 d3(-d,d,-d);
    vec3 d4(d,d,d);

    return normalize(d1*dist_field(t, p+d1)[0] + d2*dist_field(t, p+d2)[0] + 
                     d3*dist_field(t, p+d3)[0] + d4*dist_field(t, p+d4)[0]);
}

float clamp(float x, float lo, float hi) {
    return min(hi, max(lo, x));
}

float ambient(const vec3& p, const vec3& n) {
    return clamp(dot(n, light_dir), 0, 1);
}

vec3 texture(const float time, int texture_id, const vec3& pos) {
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

vec2 march(const float time, const vec3& origin, const vec3& direction, int steps) {
    vec2 res(max_dist, 0);
    float t = 1.0;

    for (int s = 0; s < steps && t < max_dist; s++) {
        res = dist_field(time, origin + t * direction);
        if (res[0] < 0.0005*t) {
            return vec2(t, res[1]);
        }
        t += res[0];
    }

    return vec2(-1, 0);
}

float shadow(float time, const vec3& p, const vec3& n, int k) {
    float t = 0.01;
    float h;
    float res = 1.0;
    vec2 dres;

    for (int s = 0; s < 16 || t < 6.0; s++) {
        dres = dist_field(time, p + t*light_dir);
        h = dres[0];
        res = min(res, k*max(0.0f, h)/t);
        if (h < 0.0001) {
            res = 0.0;
            break;
        }
        
        t += clamp(h, 0.01, 0.5);
    }
    
    return res;
}

vec3 apply_fog(const vec3& original_color, float distance, const vec3& ray_dir, const vec3& sun_dir) {
    float fog = 1.0 - exp(-powf(distance/40.0f, 2));
    float sun = max(dot(ray_dir, sun_dir), 0.0f);
    vec3 fog_color = interp(
        vec3(1.0,0.9,0.7), // sun yellow-ish
        vec3(0.5, 0.6, 0.7), // sky blue-ish
        sun
    );
    return interp(fog_color, original_color, fog);
}

color renderPixel(const float time, const vec3& camera_pos, const mat3& view_rot, const vec2& dim, const vec2& xy) {
    vec3 dir = ray_dir(45.0, view_rot, dim, vec2(xy));
    vec3 color;
    vec2 res = march(time, camera_pos, dir, max_its);

    float hit_time = res[0];
    float hit_texture = res[1];

    if (hit_time < 0) {
        color = background_color;
    } else {
        vec3 p = camera_pos + hit_time * dir;
        vec3 n = normal(time, p);

        float sha = clamp(shadow(time, p+n*0.1, n, 32), 0, 1);
        float sun = clamp(dot(n, light_dir), 0, 1);
        float sky = clamp(0.5 + 0.5*n[1], 0, 1);
        float ind = clamp(dot(n, normalize(light_dir*vec3(-1.0,0.0,-1.0))), 0, 1);

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

template<unsigned int dimx, unsigned int dimy, unsigned int min_offset, unsigned int max_offset>
inline bool is_in_range(unsigned int x, unsigned int y) {
    const auto offset = dimx * y + x;
    return min_offset <= offset && offset < max_offset;
}

template<unsigned int dimx, unsigned int dimy, unsigned int min_offset, unsigned int max_offset>
void splash_color(Screen<dimx, dimy>* screen, const std::vector<bool>& r_buffer, unsigned int x, unsigned int y, const color& c) {
    unsigned char r = std::get<0>(c), g = std::get<1>(c), b = std::get<2>(c);
    
    // this doesn't remove the artifacts since being splashed on would change the pixel :/
    if (!screen->has_changed(x, y, r, g, b)) return;

    screen->put_pixel(x, y, r, g, b);
    
    if (is_in_range<dimx, dimy, min_offset, max_offset>(x-1, y) && !r_buffer[(dimx * y) + x-1 - min_offset])
        screen->put_pixel(x-1, y, r, g, b);
    
    if (is_in_range<dimx, dimy, min_offset, max_offset>(x, y-1) && !r_buffer[(dimx * (y-1)) + x - min_offset])
        screen->put_pixel(x, y-1, r, g, b);
    
    if (is_in_range<dimx, dimy, min_offset, max_offset>(x+1, y) && !r_buffer[(dimx * y) + x + 1 - min_offset])
        screen->put_pixel(x+1, y, r, g, b);

    if (is_in_range<dimx, dimy, min_offset, max_offset>(x, y+1) && !r_buffer[(dimx * (y+1)) + x - min_offset])
        screen->put_pixel(x, y+1, r, g, b);
}

template<unsigned int dimx, unsigned int dimy, unsigned int min_offset, unsigned int max_offset>
void painter_thread(unsigned int thread_id, Screen<dimx, dimy>* screen, const vec3* camera_pos, const mat3* camera_rot, const bool* quit, const unsigned long* frame_number, const float* time) {
    constexpr unsigned int range = max_offset - min_offset;
    const vec2 dim(dimx, dimy);

    unsigned long current_frame = 0;

    std::vector<bool> r_buffer(range);
    std::fill(r_buffer.begin(), r_buffer.end(), false);

    while (!*quit) {
        if (current_frame != *frame_number) {
            std::fill(std::begin(r_buffer), std::end(r_buffer), false);
            current_frame = *frame_number;
        }

        for (auto i = 0; i < 10000; i++) {
            const unsigned int local_offset = rand() % range;
            const unsigned int offset = min_offset + local_offset;

            if (r_buffer[local_offset]) continue;
            const unsigned int x = offset % dimx;
            const unsigned int y = (offset - x) / dimx;

            color c = renderPixel(*time, *camera_pos, *camera_rot, dim, vec2(x, dimy - y - 1));
            r_buffer[local_offset] = true;

            splash_color<dimx, dimy, min_offset, max_offset>(screen, r_buffer, x, y, c);
        }
    }
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    constexpr vec2 dim(dimx, dimy);

    const unsigned int seconds_between_logs = 1;
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
    
    constexpr unsigned int num_threads = 8;
    constexpr unsigned int pixels_per_thread = dimx * dimy / num_threads;

    // some template magic could probably help me run this loop sort of statically
    // otherwise the template parameter is not proper
    DEF_RENDER_THREAD(0); DEF_RENDER_THREAD(1);
    DEF_RENDER_THREAD(2); DEF_RENDER_THREAD(3);
    DEF_RENDER_THREAD(4); DEF_RENDER_THREAD(5);
    DEF_RENDER_THREAD(6); DEF_RENDER_THREAD(7);

    while(!state.quit) {
        poll_state(state);

        view_rot += (state.left - state.right) * turn_speed;
        view_rot = fmodf(view_rot, 2.0 * M_PI);

        rot = rotationY(view_rot);

        vec3 walk_dir = vec3(0, 0, (state.down - state.up) * walk_speed);
        camera_pos = camera_pos + rot * walk_dir;

        screen.render();
        perf.tick();
        screen.sleep(100);
        frame_number++;
        time += 100.f / 1000.0f;
    }

    t0.join(); t1.join(); t2.join(); t3.join();
    t4.join(); t5.join(); t6.join(); t7.join();
    
    return EXIT_SUCCESS;
}

#include <cmath>
#include <tuple>
#include <iostream>
#include <iomanip>
#include <array>

#include "distances.hpp"
#include "transformations.hpp"
#include "mat3.hpp"
#include "vec.hpp"

#include "Screen.hpp"
#include "Controls.hpp"
#include "PerformanceMonitor.hpp"

using namespace std;

typedef std::tuple<unsigned char,unsigned char,unsigned char> color;

#define DEG_TO_RAD (M_PI / 180)

const float grad_step = 0.01;
const float max_dist = 10000.0f;
const int max_its = 256;

const vec3 light_dir = normalize(vec3(-0.3, 0.3, 0.1));
vec3 background_color(0.4,0.56,0.97);

vec3 ray_dir(float fov, const vec2& size, const vec2& pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan((90.0 - fov * 0.5) * DEG_TO_RAD);
	float z = size[1] * 0.5 * cot_half_fov;
	
	return normalize(vec3(xy, -z));
}

vec2 dist_field(const vec3& p) {
    vec3 pt;
    float d = max_dist;
    
    // floor
    pt = p;
    d = dist_plane(vec3(0,1,0), 0, pt);
    vec2 rp = vec2(d, /* texture */ 1);

    // box
    pt = translate(vec3(.75,.75,-.5), p);
    d = dist_box(vec3(1, 0.2, 1), pt);
    vec2 rb = vec2(d, /* texture */ 2);
    
    // sphere
    pt = translate(vec3(-1.5,1.5,-2.5), p);
    pt = repeatLim(1.1, 1, pt);
    d = dist_sphere(0.5, pt);
    vec2 rs = vec2(d, /* texture */ 3);
    
    return dist_union(rp, rb, rs);
}

vec3 normal(const vec3& p) {
    float d = 0.5773*0.0005;
    vec3 d1(d,-d,-d);
    vec3 d2(-d,-d,d);
    vec3 d3(-d,d,-d);
    vec3 d4(d,d,d);

    return normalize(d1*dist_field(p+d1)[0] + d2*dist_field(p+d2)[0] + 
                     d3*dist_field(p+d3)[0] + d4*dist_field(p+d4)[0]);
}

float clamp(float x, float lo, float hi) {
    return min(hi, max(lo, x));
}

float ambient(const vec3& p, const vec3& n) {
    return clamp(dot(n, light_dir), 0, 1);
}

vec3 texture(int texture_id, const vec3& pos) {
    if (texture_id == 1) { // floor
        float x = pos[0] >= 0 ? pos[0] : -pos[0] + 0.5;
        float z = pos[2] >= 0 ? pos[2] : -pos[2] + 0.5;
        return (fmod(x, 1) < 0.5) == (fmod(z, 1) < 0.5) ?
vec3(1,1,1) : vec3(0,0,0);
    } else if (texture_id == 2) { // block
        return vec3(51, 255, 189)/255.0f;
    } else if (texture_id == 3) { // sphere
        return vec3(255, 189, 51)/255.0f;
    }
    return vec3(0,1,0);
}

vec2 march(const vec3& origin, const vec3& direction, int steps) {
    vec2 res(max_dist, 0);
    float t = 1.0;

    for (int s = 0; s < steps && t < max_dist; s++) {
        res = dist_field(origin + t * direction);
        if (res[0] < 0.0005*t) {
            return vec2(t, res[1]);
        }
        t += res[0];
    }

    return vec2(-1, 0);
}

float shadow(const vec3& p, const vec3& n) {
    float t = 0.01;
    float h;
    float res = 1.0;
    vec2 dres;

    for (int s = 0; s < 32 || t <3.0; s++) {
        dres = dist_field(p + t*light_dir);
        h = dres[0];
        res = min(res, 5*h/t);
        if (h < 0.0001) {
            res = 0.1;
            break;
        }
        
        t += h;
    }
    
    return res;
}

color renderPixel(const vec3& camera_pos, const vec2& dim, const vec2& xy) {
    vec3 dir = ray_dir(45.0, dim, vec2(xy));
    vec3 color;
    vec2 res = march(camera_pos, dir, max_its);

    float hit_time = res[0];
    float hit_texture = res[1];

    if (hit_time < 0) {
        color = background_color;
    } else {
        vec3 p = camera_pos + hit_time * dir;
        vec3 n = normal(p);

        float light = 1.0;
        light *= ambient(p, n);
        light *= shadow(p, n);

        float fog = exp(-0.0005*hit_time*hit_time*hit_time);

        color = fog * light * texture((int)hit_texture, p);
    }

    color = 255.0f * color;


    return std::make_tuple(
        (unsigned char)(int)color[0],
        (unsigned char)(int)color[1],
        (unsigned char)(int)color[2]
    );
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    constexpr vec2 dim(dimx, dimy);

    const unsigned int seconds_between_logs = 1;
    const float speed = 0.1f;
    
    vec3 camera_pos = vec3(0.0, 1.0, 4.5);
    
    Screen<dimx, dimy> screen;
    controles_state state;
    PerformanceMonitor perf_monitor(seconds_between_logs);
    
    if (!screen.initialize("")) {
        return -1;
    }

    std::array<bool, dimx * dimy> rendered_buffer {};
    unsigned int pixels_rendered_in_frame = 0;

    while(!state.quit) {
        poll_state(state);
        camera_pos[0] += (state.right - state.left) * speed;
        camera_pos[2] += (state.down - state.up) * speed;

        if (state.down || state.right || state.left || state.up) {
            std::fill(std::begin(rendered_buffer), std::end(rendered_buffer), false);
        }

        pixels_rendered_in_frame = 0;
        for (int i = 0; i < 20000; i++) {
            const unsigned int x = rand() % dimx;
            const unsigned int y = rand() % dimy;

            const unsigned int offset = (dimx * y) + x;
            if(rendered_buffer[offset]) continue;

            pixels_rendered_in_frame++;

            color c = renderPixel(camera_pos, dim, vec2(x, dimy - y - 1));
            rendered_buffer[offset] = true;

            screen.put_pixel(x, y, std::get<0>(c), std::get<1>(c), std::get<2>(c));
            if (x > 0 && !rendered_buffer[(dimx * y) + x-1])
                screen.put_pixel(x-1, y, std::get<0>(c), std::get<1>(c), std::get<2>(c));
            if (y > 0 && !rendered_buffer[(dimx * (y-1)) + x])
                screen.put_pixel(x, y-1, std::get<0>(c), std::get<1>(c), std::get<2>(c));
            if (x < dimx - 1 && !rendered_buffer[(dimx * y) + x + 1])
                screen.put_pixel(x+1, y, std::get<0>(c), std::get<1>(c), std::get<2>(c));
            if (y < dimy - 1 && !rendered_buffer[(dimx * (y+1)) + x])
                screen.put_pixel(x, y+1, std::get<0>(c), std::get<1>(c), std::get<2>(c));
        }

        screen.render();
        perf_monitor.tick(pixels_rendered_in_frame);
    }
    
    return EXIT_SUCCESS;
}

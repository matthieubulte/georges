#include <fstream>
#include <iostream>
#include <cstdio>
#include <cmath>

#include "mat3.hpp"
#include "vec3.hpp"
#include "vec2.hpp"

using namespace std;

#define DEG_TO_RAD (M_PI / 180)

const float grad_step = 0.01;
const float max_dist = 10000.0f;
const int max_its = 256;

const vec3 light_dir = normalize({-0.3, 0.3, 0.1});
vec3 background_color(0.4,0.56,0.97);

vec3 ray_dir(float fov, const vec2& size, const vec2& pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan((90.0 - fov * 0.5) * DEG_TO_RAD);
	float z = size.y * 0.5 * cot_half_fov;
	
	return normalize(vec3(xy, -z));
}

vec3 pos(const vec3& v) {
    return vec3(std::max(0.0f, v.x), std::max(0.0f, v.y), std::max(0.0f, v.z));
}

void fragColor(ofstream& ofs, const vec3& _c) {
    vec3 c = pos(_c * 255.0);
    ofs << (char)((int)c.x) << (char)((int)c.y) << (char)((int)c.z);
}

mat3 viewRotation(const vec2& angles) {
    vec2 c = apply(angles, cos);
	vec2 s = apply(angles, sin);
	
    return mat3(
		s.y , s.y * s.x, s.y * c.x,
		0.0 , c.x      , -s.x,
        -s.y, c.y * s.x, c.y * c.x
	);
}

vec3 translate(const vec3& d, const vec3& p) {
    return p - d;
}

vec3 symX(const vec3& p) {
    vec3 q = p;
    q.x = abs(p.x);
    return q;
}

vec3 symXZ(const vec3& p) {
    vec3 q = p;
    q.x = abs(p.x);
    q.z = abs(p.z);
    return q;
}

vec3 repeatXZ(const vec2& pattern, const vec3& p) {
    return {
        fmod(p.x + 0.5f * pattern.x, pattern.x) - 0.5f * pattern.x,
        p.y,
        fmod(p.z + 0.5f * pattern.y, pattern.y) - 0.5f * pattern.y,
    };
}

vec3 repeatXY(const vec2& pattern, const vec3& p) {
    return {
        fmod(p.x + 0.5f * pattern.x, pattern.x) - 0.5f * pattern.x,
        fmod(p.y + 0.5f * pattern.y, pattern.y) - 0.5f * pattern.y,
        p.z,
    };
}

vec3 repeatLim(float c, float l, const vec3& p) {
    return p-c*vclamp(apply(p/c, round),-l,l);
}

float dist_sphere(float r, const vec3& p) {
    return len(p) - r;
}

float dist_box(const vec3& b, const vec3& p) {
  vec3 q = apply(p, abs) - b;
  return len(vmax(q,0.0f)) + min(max(q.x,max(q.y,q.z)),0.0f);
}

float dist_torus(const vec2& t, const vec3& p) {
  vec2 q = vec2(len(vec2(p.x, p.z))-t.x,p.y);
  return len(q)-t.y;
}

float dist_plane(const vec3& n, float h, const vec3& p) {
  return dot(p,n) + h;
}

float displacement(const vec3& p) {
    return sin(20*p.x)*sin(20*p.y)*sin(20*p.z);
}

vec2 dist_union(const vec2& res1, const vec2& res2) {
    return res1.x < res2.x ? res1 : res2;
}
vec2 dist_union(const vec2& res1, const vec2& res2, const vec2& res3) {
    return dist_union(res1, dist_union(res2, res3));
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
    d = dist_box({1, 0.2, 1}, pt);
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

    return normalize(d1*dist_field(p+d1).x + d2*dist_field(p+d2).x + 
                     d3*dist_field(p+d3).x + d4*dist_field(p+d4).x);
}

vec3 interp_color(vec3 lo, vec3 hi, float a) {
    return (1-a)*lo + a*hi;
}

float clamp(float x, float lo, float hi) {
    return min(hi, max(lo, x));
}

float ambient(const vec3& p, const vec3& n) {
    return clamp(dot(n, light_dir), 0, 1);
}
vec3 texture(int texture_id, const vec3& pos) {
    if (texture_id == 1) { // floor
        float x = pos.x >= 0 ? pos.x : -pos.x + 0.5;
        float z = pos.z >= 0 ? pos.z : -pos.z + 0.5;
        return (fmod(x, 1) < 0.5) == (fmod(z, 1) < 0.5) ?
vec3(1,1,1) : vec3(0,0,0);
    } else if (texture_id == 2) { // torus
        return vec3(51/255.0f, 255/255.0f, 189/255.0f);
    } else if (texture_id == 3) { // sphere
        return vec3(255/255.0f, 189/255.0f, 51/255.0f);
    }
    return vec3(0,1,0);
}

vec2 march(const vec3& origin, const vec3& direction, int steps) {
    vec2 res(max_dist, 0);
    float t = 1.0;

    for (int s = 0; s < steps && t < max_dist; s++) {
        res = dist_field(origin + t * direction);
        if (res.x < 0.0005*t) {
            return vec2(t, res.y);
        }
        t += res.x;
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
        h = dres.x;
        res = min(res, 5*h/t);
        if (h < 0.0001) {
            res = 0.1;
            break;
        }
        
        t += h;
    }
    
    return res;
}

void renderPixel(ofstream& ofs, const vec3& camera_pos, const vec2& dim, const vec2& xy) {
    vec3 dir = ray_dir(45.0, dim, vec2(xy));
    vec3 color;
    vec2 res = march(camera_pos, dir, max_its);
    float hit_time = res.x;
    float hit_texture = res.y;

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

    fragColor(ofs, color);
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u;
    constexpr vec2 dim(dimx, dimy);

	vec3 camera_pos = vec3( 0.0, 1.0, 4.5 );
    vec2 angles = vec2(0,0); //-0.5 * flip(dim) * vec2(0.01, -0.01);
    mat3 view_rot = viewRotation(angles); // todo, this is broken, fix it

    ofstream ofs("image.ppm", ios_base::out | ios_base::binary);
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;
 
    for (int y = dimy-1; y >= 0; --y) {
        for (auto x = 0u; x < dimx; ++x) {
            renderPixel(ofs, camera_pos, dim, vec2(x, y));
        }
    }

    ofs.close();
 
    return EXIT_SUCCESS;
}

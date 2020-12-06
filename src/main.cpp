#include <fstream>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <array>

#include <SDL2/SDL.h>

#include "distances.hpp"
#include "transformations.hpp"
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

void fragColor(unsigned char* dest, const vec3& _c) {
    vec3 c = pos(_c * 255.0);
    dest[0] = (unsigned char)(int)c.z; // blue
    dest[1] = (unsigned char)(int)c.y; // green
    dest[2] = (unsigned char)(int)c.x; // red
    
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
    } else if (texture_id == 2) { // block
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

void renderPixel(unsigned char* dest, const vec3& camera_pos, const vec2& dim, const vec2& xy) {
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

    fragColor(dest, color);
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    unsigned int pixels_per_frame = 1000u;

    constexpr vec2 dim(dimx, dimy);
    vec3 camera_pos = vec3(0.0, 1.0, 4.5);
    
    bool quit = false;
    SDL_Event e;

    array<unsigned char, dimx  * dimy * channels> framebuffer{};

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else {
        window = SDL_CreateWindow("", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            dimx, dimy,
            SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, dimx, dimy);

        //While application is running
        int frame_number = 0;
        Uint64 start = SDL_GetPerformanceCounter();

        int dirx = 0, dirz = 0;

        while(!quit) {
            //Handle events on queue
            while(SDL_PollEvent(&e) != 0 ) {
                switch (e.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_KEYDOWN:
                        switch(e.key.keysym.sym ){
                            case SDLK_LEFT:
                                dirx = -1;
                                break;
                            case SDLK_RIGHT:
                                dirx = 1;
                                break;
                            case SDLK_UP:
                                dirz = -1;
                                break;
                            case SDLK_DOWN:
                                dirz = 1;
                                break;
                            default: break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch(e.key.keysym.sym ){
                            case SDLK_LEFT:
                                dirx = 0;
                                break;
                            case SDLK_RIGHT:
                                dirx = 0;
                                break;
                            case SDLK_UP:
                                dirz = 0;
                                break;
                            case SDLK_DOWN:
                                dirz = 0;
                                break;
                            default: break;
                        }
                        break;

                    default: break;
                };
            }

            const float speed = 0.1f;
            camera_pos.x += dirx * speed;
            camera_pos.z += dirz * speed;

            for (int i = 0; i < pixels_per_frame; i++) {
                const unsigned int x = rand() % dimx;
                const unsigned int y = rand() % dimy;
                const unsigned int offset = (dimx * channels * y) + x * channels;
                renderPixel(&framebuffer[offset], camera_pos, dim, vec2(x, dimy - y - 1));
            }

            SDL_UpdateTexture (
                texture,
                NULL,
                framebuffer.data(),
                dimx * channels
                );

            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            frame_number++;
            const Uint64 end = SDL_GetPerformanceCounter();
            const static Uint64 freq = SDL_GetPerformanceFrequency();
            const double seconds = ( end - start ) / static_cast< double >( freq );
            const double ms_per_frame = ( seconds * 1000.0 ) / frame_number;

            if (seconds > 2) {
                const double ms_per_pixel = (ms_per_frame / (double)pixels_per_frame);
                const double target_ms_per_frame = 1000.0/20.0f;
                pixels_per_frame = (unsigned int)(target_ms_per_frame / ms_per_pixel);

                cout
                    << frame_number << " frames in "
                    << setprecision(1) << fixed << seconds << " seconds = "
                    << setprecision(1) << fixed << frame_number / seconds << " FPS ("
                    << setprecision(3) << fixed << ms_per_frame << " ms/frame, "
                    << setprecision(3) << fixed << ms_per_pixel << " ms/pixel) new pixels_per_frame = "
                    << setprecision(1) << fixed << pixels_per_frame
                    << endl;
                start = end;
                frame_number = 0;
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyWindow( window );
    SDL_Quit();
 
    return EXIT_SUCCESS;
}

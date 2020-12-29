#include <cmath>
#include <tuple>
#include <iostream>
#include <iomanip>
#include <array>
#include <thread>
#include <vector>

#include "distances.hpp"
#include "transformations.hpp"
#include "linalg/mat3.hpp"
#include "linalg/vec.hpp"
#include "linalg/vecpack.hpp"

#include "screen.hpp"
#include "scenes/simple_scene.hpp"
#include "camera.hpp"
#include "painter.hpp"
#include "shader.hpp"
#include "controls.hpp"
#include "performance_monitor.hpp"


#define SIMD
// #define MULTITHREADED

#define DEF_RENDER_THREAD(i) \
    Painter<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread> painter##i(&screen, &shader);\
    std::thread t##i(painter_thread<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread>, &painter##i, &state.quit);

template<size_t screen_width, size_t screen_height, size_t min_offset, size_t max_offset>
void painter_thread(Painter<screen_width, screen_height, min_offset, max_offset>* painter, const bool* quit) {
    while (!*quit) {
        #ifdef SIMD
        painter->paint_simd(1000);
        #else
        painter->paint(8000);
        #endif
    }
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    const vec2 dim(dimx, dimy);

    const size_t seconds_between_logs = 1;
    const float walk_speed = 0.2f;
    const float turn_speed = 0.05f;
    vec3 walk_dir;

    ShaderConfig shader_config;    
    shader_config.max_dist = 10000.0f;
    shader_config.max_its = 256;
    shader_config.light_dir = normalize(vec3(-0.2, 0.2, 0));
    shader_config.background_color = vec3(0.4,0.56,0.97);
    shader_config.time = 0.0f;

    SimpleScene scene;

    Screen<dimx, dimy> screen;
    Camera camera(45.0f, dim, vec3(0.0, 1.0, 0.0), -M_PI);
    Shader shader(&shader_config, &camera, &scene);
    Painter<dimx, dimy, 0, dimx * dimy> painter(&screen, &shader);
    PerformanceMonitor perf(2);
    controles_state state;
    
    #ifdef SIMD
    const char* title = "SIMD implementation";
    #else
    const char* title = "Reference implementation";
    #endif

    if (!screen.initialize(title)) return -1;

    std::cout << "RUNNING: " << title << std::endl;
    
    constexpr size_t num_threads = 8;
    constexpr size_t pixels_per_thread = dimx * dimy / num_threads;

    // some template magic could probably help me run this loop sort of statically
    // otherwise the template parameter is not proper
    #ifdef MULTITHREADED
    DEF_RENDER_THREAD(0); DEF_RENDER_THREAD(1);
    DEF_RENDER_THREAD(2); DEF_RENDER_THREAD(3);
    DEF_RENDER_THREAD(4); DEF_RENDER_THREAD(5);
    DEF_RENDER_THREAD(6); DEF_RENDER_THREAD(7);
    #endif

    while(!state.quit) {
        poll_state(state);

        camera.turn((state.left - state.right) * turn_speed);

        walk_dir = vec3(0, 0, (state.down - state.up) * walk_speed);
        camera.move_forward(walk_dir);

        #ifdef MULTITHREADED
        screen.sleep(18);
        shader_config.time += 18;
        #else
        perf.tick();

        #ifdef SIMD
        painter.paint_simd(1000);
        #else
        painter.paint(8000);
        #endif
        
        shader_config.time += perf.tock();
        #endif

        screen.render();
    }

    #ifdef MULTITHREADED
    t0.join(); t1.join(); t2.join(); t3.join();
    t4.join(); t5.join(); t6.join(); t7.join();
    #endif

    return EXIT_SUCCESS;
}

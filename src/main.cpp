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
#include "vecpack.hpp"

#include "screen.hpp"
#include "camera.hpp"
#include "painter.hpp"
#include "shader.hpp"
#include "controls.hpp"
#include "performance_monitor.hpp"


#define DEF_RENDER_THREAD(i) \
    Painter<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread, ENABLE_PIXEL_CACHE> painter##i(&screen, &shader);\
    std::thread t##i(painter_thread<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread>, &painter##i, &state.quit);

template<size_t screen_width, size_t screen_height, size_t min_offset, size_t max_offset>
void painter_thread(Painter<screen_width, screen_height, min_offset, max_offset>* painter, const bool* quit) {
    while (!*quit) {
        painter->paint_simd(1000);
    }
}

int main() {
    constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    constexpr vec2 dim(dimx, dimy);

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

    Screen<dimx, dimy> screen;
    Camera camera(45.0f, dim, vec3(0.0, 1.0, 0.0), M_PI);
    Shader shader(&shader_config, &camera);
    Painter<dimx, dimy, 0, dimx * dimy> painter(&screen, &shader);
    PerformanceMonitor perf(2);
    controles_state state;
    
    if (!screen.initialize("")) return -1;
    
    constexpr size_t num_threads = 8;
    constexpr size_t pixels_per_thread = dimx * dimy / num_threads;

    // some template magic could probably help me run this loop sort of statically
    // otherwise the template parameter is not proper
    // DEF_RENDER_THREAD(0); DEF_RENDER_THREAD(1);
    // DEF_RENDER_THREAD(2); DEF_RENDER_THREAD(3);
    // DEF_RENDER_THREAD(4); DEF_RENDER_THREAD(5);
    // DEF_RENDER_THREAD(6); DEF_RENDER_THREAD(7);

    while(!state.quit) {
        poll_state(state);

        camera.turn((state.left - state.right) * turn_speed);

        walk_dir = vec3(0, 0, (state.down - state.up) * walk_speed);
        camera.move_forward(walk_dir);

        perf.tick();
        painter.paint_simd(1000);
        // painter.paint(8000);
        shader_config.time += perf.tock();

        screen.render();
        // screen.sleep(18);
    }

    // t0.join(); t1.join(); t2.join(); t3.join();
    // t4.join(); t5.join(); t6.join(); t7.join();
    
    return EXIT_SUCCESS;
}

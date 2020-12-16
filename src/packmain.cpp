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


#define ENABLE_PIXEL_CACHE false

#define DEF_RENDER_THREAD(i) \
    Painter<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread, ENABLE_PIXEL_CACHE> painter##i(&screen, &shader);\
    std::thread t##i(painter_thread<dimx, dimy, i*pixels_per_thread, (i+1)*pixels_per_thread, ENABLE_PIXEL_CACHE>, &painter##i, &state.quit);

template<size_t screen_width, size_t screen_height, size_t min_offset, size_t max_offset, bool cache>
void painter_thread(Painter<screen_width, screen_height, min_offset, max_offset, cache>* painter, const bool* quit) {
    while (!*quit) {
        painter->paint(10000);
    }
}

#include <chrono> 
using namespace std::chrono; 


volatile float c;

template<size_t N_vecs>
__attribute__((noinline)) vecpack<N_vecs, 2> dist_field_packed(const vecpack<N_vecs, 3>& p) {
    auto start = high_resolution_clock::now(); 

    std::cout << "p: [";
    for (auto i : p.data) std::cout << i << ",";
    std::cout << "]" << std::endl;

    vecpack<N_vecs, 3> q;
    std::cout << "q: [";
    for (auto i : q.data) std::cout << i << ",";
    std::cout << "]" << std::endl;

    vec<N_vecs> d(10000.0f);
    std::cout << "d: [";
    std::cout << d << ",";
    std::cout << "]" << std::endl;
    
    // floor
    d = dist_plane(vec3(0,1,0), 0, p);
    std::cout << "d: [";
    std::cout << d << ",";
    std::cout << "]" << std::endl;

    // sphere
    q = p - vec3(0.0f, 1.0f, 0.0f);
    std::cout << "q: [";
    for (auto i : q.data) std::cout << i << ",";
    std::cout << "]" << std::endl;

    vec<N_vecs> d2 = dist_sphere(0.5f, q);
    std::cout << "d2: [";
    std::cout << d2 << ",";
    std::cout << "]" << std::endl;

    vec<N_vecs> ds = smin(d, d2, 0.32);
    std::cout << "ds: [";
    std::cout << ds << ",";
    std::cout << "]" << std::endl;

    vecpack<N_vecs, 2> res;
    res[0] = ds;
    res[1] = 1.0f;

    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
    c += duration.count();

    return res;
}

__attribute__((noinline)) vecpack<8, 2> dist_field_regular(const std::array<vec3, 8>& points) {
    auto start = high_resolution_clock::now(); 
    vecpack<8, 2> res; 
    int i = 0;
    for (const vec3& p : points) {
        vec3 q;
        float d = 10000.0f;
        
        // floor
        d = dist_plane(vec3(0,1,0), 0, p);

        // sphere
        q = p - vec3(0.0f, 1.0f, 0.0f);
        float d2 = dist_sphere(0.5f, q);
        float ds = smin(d, d2, 0.32);
        
        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start); 
        c += duration.count();

        res[0][i] = ds;
        res[1][i++] = 1.0;
    }

    return res;
}

void memcpy(volatile float* dst, float* src, size_t len) {
    while(len) { dst[len-1] = src[len-1]; len--; }
}

#include <chrono> 
using namespace std::chrono; 


int main() {
    // constexpr auto dimx = 1280u, dimy = 720u, channels = 4u;
    // constexpr vec2 dim(dimx, dimy);

    // const size_t seconds_between_logs = 1;
    // const float walk_speed = 0.2f;
    // const float turn_speed = 0.05f;
    // vec3 walk_dir;

    // ShaderConfig shader_config;    
    // shader_config.max_dist = 10000.0f;
    // shader_config.max_its = 256;
    // shader_config.light_dir = normalize(vec3(-0.2, 0.2, 0));
    // shader_config.background_color = vec3(0.4,0.56,0.97);
    // shader_config.time = 0.0f;

    // Screen<dimx, dimy> screen;
    // Camera camera(45.0f, dim, vec3(0.0, 1.0, 0.0), M_PI);
    // Shader shader(&shader_config, &camera);
    // Painter<dimx, dimy, 0, dimx * dimy, false> painter(&screen, &shader);
    // PerformanceMonitor perf(100);
    // controles_state state;
    
    // if (!screen.initialize("")) return -1;
    
    // constexpr size_t num_threads = 8;
    // constexpr size_t pixels_per_thread = dimx * dimy / num_threads;
    // some template magic could probably help me run this loop sort of statically
    // otherwise the template parameter is not proper
    // DEF_RENDER_THREAD(0); DEF_RENDER_THREAD(1);
    // DEF_RENDER_THREAD(2); DEF_RENDER_THREAD(3);
    // DEF_RENDER_THREAD(4); DEF_RENDER_THREAD(5);
    // DEF_RENDER_THREAD(6); DEF_RENDER_THREAD(7);

    // while(!state.quit) {
    //     poll_state(state);

    //     camera.turn((state.left - state.right) * turn_speed);

    //     walk_dir = vec3(0, 0, (state.down - state.up) * walk_speed);
    //     camera.move_forward(walk_dir);

    //     painter.paint(10000);

    //     screen.render();
        
    //     shader_config.time += perf.tick();
    // }

    std::array<vec3, 8> points;
    for (auto i = 0; i < 8; i++) {
        points[i] = vec3(
            (rand() % 10000)/100.0f,
            (rand() % 10000)/100.0f,
            (rand() % 10000)/100.0f);
    }

    vecpack<8, 3> packed;
    for (auto i = 0; i < 8; i++) {
        packed[0][i] = points[i][0];
        packed[1][i] = points[i][1];
        packed[2][i] = points[i][2];
    }

    size_t workload = 1000000, N = 1;
    

    volatile float* a = new float[1000];

    {
        float total = 0.0f, total_sq = 0.0f, duration;
        for (auto i = 0; i < N; i++) {
            c = 0.0f;
            
            for (auto w = 0; w < workload; w++) {
                vecpack<8, 2> res = dist_field_regular(points);
                memcpy(a, res[0].data.data(), 8);
                memcpy(a, res[1].data.data(), 8);

                if (w == workload - 1) {
                    std::cout << res[0] << std::endl;
                    std::cout << res[1] << std::endl;
                }
            }

            duration = c;
            total += duration;
            total_sq += duration*duration;
        }

        float mean = (total / N) / 1000.0f;
        float var = total_sq / 1000.0f / 1000.0f / N - mean*mean;

        std::cout << "Regular: " << mean << "ms (var=" << var << "ms) per " << workload << " distances " << std::endl;
    }

    {
        float total = 0.0f, total_sq = 0.0f, duration;
        for (auto i = 0; i < N; i++) {
            c = 0.0f;

            for (auto w = 0; w < workload; w++) {
                vecpack<8, 2> res = dist_field_packed(packed);
                memcpy(a, res[0].data.data(), 8);
                memcpy(a, res[1].data.data(), 8);

                if (w == workload - 1) {
                    std::cout << res[0] << std::endl;
                    std::cout << res[1] << std::endl;
                }
            }

            duration = c;
            total += duration;
            total_sq += duration*duration;
        }

        float mean = (total / N) / 1000.0f;
        float var = total_sq / N / 1000.0f / 1000.0f - mean*mean;

        std::cout << "SIMD:  " << mean << "ms (var=" << var << "ms) per " << workload << " distances " << std::endl;
    }
  
    delete[] a;

    // t0.join(); t1.join(); t2.join(); t3.join();
    // t4.join(); t5.join(); t6.join(); t7.join();
    
    return EXIT_SUCCESS;
}

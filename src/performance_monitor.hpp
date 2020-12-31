#ifndef PERFORMANCE_HELPER_HPP
#define PERFORMANCE_HELPER_HPP

#include <iostream>
#include <iomanip>
#include <SDL2/SDL.h>

class PerformanceMonitor {
    public:
    PerformanceMonitor(unsigned int seconds_between_update)
        : last_update_time(SDL_GetPerformanceCounter()),
          frame_start(SDL_GetPerformanceCounter()),
          num_frames(0),
          seconds_between_update(seconds_between_update)
           {}

    void log_performance() {
        const Uint64 frame_end = SDL_GetPerformanceCounter();
        const Uint64 freq = SDL_GetPerformanceFrequency();
        const float seconds_since_last_update = (frame_end - this->last_update_time) / static_cast<float>(freq);
        const float ms_per_frame = (seconds_since_last_update * 1000.0) / this->num_frames;

        std::cout
            << "PERF: "
            << std::setprecision(1) << std::fixed << this->num_frames / seconds_since_last_update << " fps ("
            << std::setprecision(3) << std::fixed << ms_per_frame
            << " ms/frame)"
            << std::endl;
        
        this->last_update_time = frame_end;
        this->num_frames = 0;
    }

    void tick() {
        this->frame_start = SDL_GetPerformanceCounter();
    }

    float tock() {
        this->num_frames++;

        const Uint64 frame_end = SDL_GetPerformanceCounter();
        const Uint64 freq = SDL_GetPerformanceFrequency();
        const float seconds_since_last_update = (frame_end - this->last_update_time) / static_cast<float>(freq);
        
        if (seconds_since_last_update > this->seconds_between_update) {
            log_performance();
        }

        return (frame_end - this->frame_start) / static_cast<float>(freq);
    }

    private:
    Uint64 last_update_time;
    Uint64 frame_start;
    unsigned int num_frames;
    unsigned int seconds_between_update;
};

#endif
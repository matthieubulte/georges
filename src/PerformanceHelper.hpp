#ifndef PERFORMANCE_HELPER_HPP
#define PERFORMANCE_HELPER_HPP

#include <iostream>
#include <iomanip>
#include <SDL2/SDL.h>

class PerformanceHelper {
    public:
    PerformanceHelper(unsigned int seconds_between_update, unsigned int target_frames_per_second, float* workload, bool verbose)
        : seconds_between_update(seconds_between_update),
          target_frame_duration(1000.0f / target_frames_per_second),
          verbose(verbose),
          frame_start(SDL_GetPerformanceCounter()),
          num_frames(0),
          workload(workload)
           {}

    void tick() {
        this->num_frames++;

        const Uint64 frame_end = SDL_GetPerformanceCounter();
        const Uint64 freq = SDL_GetPerformanceFrequency();
        const float seconds_since_last_update = (frame_end - this->frame_start) / static_cast<float>(freq);
        const float ms_per_frame = (seconds_since_last_update * 1000.0) / this->num_frames;
        const float perf_adjustment = this->target_frame_duration / ms_per_frame;
        *this->workload *= perf_adjustment;

        if (seconds_since_last_update > this->seconds_between_update) {
            if (this->verbose) {
                std::cout
                << "PERF: "
                << std::setprecision(1) << std::fixed << this->num_frames / seconds_since_last_update << " fps ("
                << std::setprecision(3) << std::fixed << ms_per_frame
                << " ms/frame) | new workload "
                << std::setprecision(3) << std::fixed << *workload
                << " (adjust. "
                << std::setprecision(3) << std::fixed << perf_adjustment
                << ")"
                << std::endl;
            }
            this->frame_start = frame_end;
            this->num_frames = 0;
        }
    }

    private:
    Uint64 frame_start;
    unsigned int num_frames;
    unsigned int seconds_between_update;
    float target_frame_duration;
    bool verbose;
    float* workload;
};

#endif
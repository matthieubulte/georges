#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <array>

#include "screen.hpp"
#include "shader.hpp"
#include "types.hpp"

template<size_t screen_width, size_t screen_height, size_t min_offset, size_t max_offset, bool cache>
class Painter {
    public:
    Painter(Screen<screen_width, screen_height>* screen, const Shader* shader) : screen(screen), shader(shader) {
        reset_write_cache();
    }

    void paint(size_t num_pixels) {
        size_t local_offset, offset, x, y;

        for (auto i = 0; i < num_pixels; i++) {
            local_offset = rand() % num_pixels_covered;
            offset = min_offset + local_offset;

            if (cache && write_cache[local_offset]) continue;
            x = offset % screen_width;
            y = (offset - x) / screen_width;

            color c = shader-> render_pixel(x, screen_height - y - 1);
            if (cache) write_cache[local_offset] = true;
            splash_color(x, y, c);
        }
    }

    void paint_simd(size_t num_packs) {
        size_t local_offset, offset, x, y;

        for (auto i = 0; i < num_packs; i++) {
            vecpack<8, 2> pixels;
            std::array<size_t, 8 * 2> coordinates;

            for (auto i = 0; i < 8; i++) {
                local_offset = rand() % num_pixels_covered;
                offset = min_offset + local_offset;

                x = offset % screen_width;
                y = screen_height - 1 - (offset - x) / screen_width;

                coordinates[i*2] = x;
                coordinates[i*2+1] = y;

                pixels[0][i] = x;
                pixels[1][i] = y;
            }            

            std::array<color, 8> c = shader->render_pixel_simd(pixels);
            for (auto i = 0; i < 8; i++) {
                splash_color(coordinates[i*2], coordinates[i*2+1], c[i]);
            }
        }
    }

    private:
    void reset_write_cache() {
        std::fill(write_cache.begin(), write_cache.end(), false);
    }

    void splash_color(size_t x, size_t y, const color& c) {
        screen->put_pixel(x, y, c);
        
        if (is_covered(x-1, y) && !is_cached(x-1, y)) screen->put_pixel(x-1, y, c);
        if (is_covered(x, y-1) && !is_cached(x, y-1)) screen->put_pixel(x, y-1, c);
        if (is_covered(x+1, y) && !is_cached(x+1, y)) screen->put_pixel(x+1, y, c);
        if (is_covered(x, y+1) && !is_cached(x, y+1)) screen->put_pixel(x, y+1, c);
    }

    inline bool is_covered(size_t x, size_t y) const {
        const size_t offset = screen_width * y + x;
        return min_offset <= offset && offset < max_offset;
    }

    inline bool is_cached(size_t x, size_t y) const {
        const size_t offset = screen_width * y + x;
        return write_cache[offset];
    }

    static constexpr size_t num_pixels_covered = max_offset - min_offset;

    std::array<bool, num_pixels_covered> write_cache;

    Screen<screen_width, screen_height>* screen;
    const Shader* shader;
};

#endif
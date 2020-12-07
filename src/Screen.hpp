#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SDL2/SDL.h>
#include <array>

template<size_t screen_width, size_t screen_height>
class Screen {
    public:
    Screen();
    ~Screen();

    bool initialize(const char* window_title);
    void put_pixel(const unsigned int x, const unsigned int y, unsigned char r, unsigned char g, unsigned char b);
    void render();

    private:
    bool initialized;
    
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *frame_texture;

    std::array<unsigned char, screen_width * screen_height * 4> framebuffer {};

};

template<size_t screen_width, size_t screen_height>
Screen<screen_width, screen_height>::Screen() {
}

template<size_t screen_width, size_t screen_height>
Screen<screen_width, screen_height>::~Screen() {
    if (this->initialized) {
        SDL_DestroyTexture(this->frame_texture);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        this->initialized = false;
    }
}

template<size_t screen_width, size_t screen_height>
bool Screen<screen_width, screen_height>::initialize(const char* window_title) {
    if (SDL_Init(SDL_INIT_VIDEO ) < 0) {
        printf( "Failed to initialize SDL, SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    this->window = SDL_CreateWindow(window_title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screen_width, screen_height,
        SDL_WINDOW_SHOWN);

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    this->frame_texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);

    return this->initialized = true;
}

template<size_t screen_width, size_t screen_height>
inline void Screen<screen_width, screen_height>::put_pixel(const unsigned int x, const unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
    const unsigned int offset = (screen_width * 4 * y) + x * 4;
    this->framebuffer[offset] = b;
    this->framebuffer[offset+1] = g;
    this->framebuffer[offset+2] = r;
}

template<size_t screen_width, size_t screen_height>
void Screen<screen_width, screen_height>::render() {
    SDL_UpdateTexture (this->frame_texture, NULL, this->framebuffer.data(), screen_width * 4);
    SDL_RenderCopy(this->renderer, this->frame_texture, NULL, NULL);
    SDL_RenderPresent(this->renderer);
}

#endif
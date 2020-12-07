#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <SDL2/SDL.h>

typedef struct controles_state {
    char left = 0, right = 0, up = 0, down = 0;
    bool quit = false;
} controles_state;

void poll_state(controles_state& state) {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0 ) {
        switch (e.type) {
            case SDL_QUIT:
                state.quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym ){
                    case SDLK_LEFT:
                        state.left = 1;
                        break;
                    case SDLK_RIGHT:
                        state.right = 1;
                        break;
                    case SDLK_UP:
                        state.up = 1;
                        break;
                    case SDLK_DOWN:
                        state.down = 1;
                        break;
                    default: break;
                }
                break;
            case SDL_KEYUP:
                switch(e.key.keysym.sym ){
                    case SDLK_LEFT:
                        state.left = 0;
                        break;
                    case SDLK_RIGHT:
                        state.right = 0;
                        break;
                    case SDLK_UP:
                        state.up = 0;
                        break;
                    case SDLK_DOWN:
                        state.down = 0;
                        break;
                    default: break;
                }
                break;

            default: break;
        };
    }
}

#endif
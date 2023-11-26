#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

int SCREEN_HEIGHT = 800;
int SCREEN_WIDTH = 600;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        success = false;
    }

    return success;
}

int main() {
    bool initOk = init();
    if (!initOk) {
        exit(-1);
    }

    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Mob Nerve A", 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_HIDDEN);

    SDL_Renderer* renderer = NULL;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* helloTexture = IMG_LoadTexture(renderer, "hello.png");
    if (helloTexture == NULL) {
        printf("Unable to load texture %s! SDL Error: %s\n", "hello.png", SDL_GetError());
        exit(-1);
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, helloTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_ShowWindow(window);

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;

                case SDLK_DOWN:
                    SDL_SetRenderDrawColor(renderer, 0xCC, 0xCC, 0xCC, 0xFF);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;

                case SDLK_RIGHT:
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, helloTexture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    break;

                default:
                    break;
                }
            }
        }

        SDL_Delay(32);
    }

    SDL_DestroyTexture(helloTexture);
    helloTexture = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    IMG_Quit();
    SDL_Quit();

    return 0;
}

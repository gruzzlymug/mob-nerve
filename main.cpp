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

    SDL_Window *window = SDL_CreateWindow("Mob Nerve A", 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_HIDDEN);

    SDL_Surface* helloImage = IMG_Load("hello.png");
    if (helloImage == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", "hello.png", SDL_GetError());
        exit(-1);
    }
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    SDL_BlitSurface(helloImage, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);

    SDL_ShowWindow(window);

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_Delay(32);
    }

    SDL_FreeSurface(helloImage);
    helloImage = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();

    return 0;
}

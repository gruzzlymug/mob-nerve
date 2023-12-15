#pragma once

#include "SDL2/SDL.h"
#include "matlib.h"

class Graphics {
private:
    int entity_id;

    void draw_circle(int center_x, int center_y, int radius, SDL_Renderer* renderer);

public:
    explicit Graphics(int entity_id) {
        this->entity_id = entity_id;
    }

    ~Graphics() {
    }

    Graphics(const Graphics& other) = delete;
    Graphics& operator=(const Graphics& other) = delete;

    void draw(vector4& pos, vector4& heading, vector4& right, SDL_Renderer* renderer) {
        float x = pos[0];
        float y = pos[1];
        float hx = heading[0];
        float hy = heading[1];
        float rhx = right[0];
        float rhy = right[1];

        draw_circle(x, y, 10, renderer);
        SDL_RenderDrawLine(renderer, x, y, x+10*hx, y+10*hy);
        SDL_RenderDrawLine(renderer, x, y, x+5*rhx, y+5*rhy);
    }
};

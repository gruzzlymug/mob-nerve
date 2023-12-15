#pragma once

#include "SDL2/SDL.h"
#include "matlib.h"

class Graphics {
private:
    int entity_id;

public:
    explicit Graphics(int entity_id) {
        this->entity_id = entity_id;
    }

    ~Graphics() {
    }

    Graphics(const Graphics& other) = delete;
    Graphics& operator=(const Graphics& other) = delete;

    void draw_heading(SDL_Renderer* renderer, vector4& pos, vector4& heading, vector4& right);
    void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius);
};

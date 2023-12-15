#pragma once

#include "Graphics.h"
#include "Location.h"

#include "SDL2/SDL.h"
#include "matlib.h"

class Thing {
private:
    // TODO revisit
    static int next_id;

    int id;
    Location location;
    Graphics graphics;
    int side;

public:
    Thing()
    : id(next_id++)
    , location(id)
    , graphics(id)
    {
        side = 0;
    }

    ~Thing() {
    }

    vector4 position() {
        return location.position();
    }

    vector4 heading() {
        return location.heading();
    }

    vector4 right() {
        return location.right();
    }

    void move(float x, float y, float z) {
        location.move(x, y, z);
    }

    void rotate(float deg) {
        location.rotate(deg);
    }

    void set_side(int s) {
        side = s;
    }

    int get_side() {
        return side;
    }

    void draw(vector4& pos, vector4& heading, vector4& right, SDL_Renderer* renderer) {
        graphics.draw(pos, heading, right, renderer);
    }
};

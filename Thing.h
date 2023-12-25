#pragma once

#include "Graphics.h"
#include "Location.h"
#include "SDL2/SDL.h"
#include "matlib.h"

class Thing {
private:
    static int next_id;

    int id;
    int side;
    float visibility;
    Location location;
    Graphics graphics;

public:
    Thing()
    : id(next_id++)
    , location(id)
    , graphics(id)
    {
        side = 0;
        visibility = 0;
    }

    ~Thing() {
    }

    vector4 position() {
        return location.get_position();
    }

    vector4 heading() {
        matrix44 transform = location.get_transform();
        return transform[0];
    }

    vector4 right() {
        matrix44 transform = location.get_transform();
        return transform[1];
    }

    void move(float x, float y, float z) {
        vector4 position = location.get_position();
        location.set_position(position.x + x, position.y + y, position.z + z);
    }

    void rotate(char axis, float deg) {
        float cur_deg = 0;
        switch (axis) {
            case 'x':
                cur_deg = location.get_pitch();
                location.set_pitch(cur_deg + deg);
                break;
            case 'y':
                cur_deg = location.get_yaw();
                location.set_yaw(cur_deg + deg);
                break;
            case 'z':
                cur_deg = location.get_roll();
                location.set_roll(cur_deg + deg);
                break;
        }
    }

    void set_side(int s) {
        side = s;
    }

    int get_side() {
        return side;
    }

    void set_visibility(float v) {
        visibility = v;
    }

    float get_visibility() {
        return visibility;
    }

    void draw(vector4& pos, vector4& heading, vector4& right, SDL_Renderer* renderer) {
        graphics.draw_circle(renderer, pos[0], pos[1], 10 + 5 * visibility);
        graphics.draw_heading(renderer, pos, heading, right);
    }
};

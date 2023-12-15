#pragma once

#include "matlib.h"
#include "SDL2/SDL.h"

class Location {
private:
    // TODO investigate alignment
    int entity_id;
    matrix44 location;

public:
    explicit Location(int entity_id) {
        this->entity_id = entity_id;
        location = IdentityMatrix44();
    }

    ~Location() {
    }

    Location(const Location& other) = delete;
    Location& operator=(const Location& other) = delete;

    vector4 position() {
        return location[3];
    }

    vector4 heading() {
        return location[0];
    }

    vector4 right() {
        return location[1];
    }

    void move(float x, float y, float z) {
        location = location * TranslateMatrix44(x, y, z);
    }

    void rotate(float deg) {
        location = location * RotateRadMatrix44('z', deg*3.14159/180.0f);
    }
};

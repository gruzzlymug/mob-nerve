#pragma once

#include "matlib.h"
#include "SDL2/SDL.h"

class Location {
private:
    // TODO investigate alignment
    int entity_id;
    float yaw;
    float pitch;
    float roll;
    vector4 position;

public:
    explicit Location(int entity_id) {
        this->entity_id = entity_id;
        yaw = 0.0f;
        pitch = 0.0f;
        roll = 0.0f;
        position = vector4{0.0f, 0.0f, 0.0f, 1.0f};
    }

    ~Location() {
    }

    Location(const Location& other) = delete;
    Location& operator=(const Location& other) = delete;

    float get_yaw() {
        return yaw;
    }

    void set_yaw(float deg) {
        yaw = deg;
    }

    float get_pitch() {
        return pitch;
    }

    void set_pitch(float deg) {
        pitch = deg;
    }

    float get_roll() {
        return roll;
    }

    void set_roll(float deg) {
        roll = deg;
    }

    vector4 get_position() {
        return position;
    }

    void set_position(float x, float y, float z) {
        position[0] = x;
        position[1] = y;
        position[2] = z;
        position[3] = 1.0f;
    }

    const matrix44 get_transform() const {
        matrix44 translate = TranslateMatrix44(position.x, position.y, position.z);
        matrix44 yawMat = RotateRadMatrix44('y', yaw * M_PI / 180.0f);
        matrix44 rollMat = RotateRadMatrix44('z', roll * M_PI / 180.0f);
        matrix44 transform = translate * yawMat * rollMat;
        // transform = matlib::rotate(transform, pitch, vector4{1.0f, 0.0f, 0.0f, 0.0f});
        return transform;
    }
};

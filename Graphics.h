#pragma once

#include "matlib.h"
#include <memory>

class Mesh;
class SDL_Renderer;

class Graphics {
private:
    int entity_id;
    std::shared_ptr<Mesh> mesh;
    // std::shared_ptr<SDL_Renderer> renderer;

public:
    explicit Graphics(int entity_id) {
        this->entity_id = entity_id;
        mesh = nullptr;
    }

    ~Graphics() {
    }

    Graphics(const Graphics& other) = delete;
    Graphics& operator=(const Graphics& other) = delete;

    void set_mesh(std::shared_ptr<Mesh> mesh) {
        this->mesh = mesh;
    }

    std::shared_ptr<Mesh> get_mesh() {
        return mesh;
    }

    void draw_heading(SDL_Renderer* renderer, vector4& pos, vector4& heading, vector4& right);
    void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius);
};

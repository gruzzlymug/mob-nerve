#include "Graphics.h"

void Graphics::draw_heading(SDL_Renderer* renderer, vector4& pos, vector4& heading, vector4& right) {
    float x = pos[0];
    float y = pos[1];
    float hx = heading[0];
    float hy = heading[1];
    float rhx = right[0];
    float rhy = right[1];

    SDL_RenderDrawLine(renderer, x, y, x+10*hx, y+10*hy);
    SDL_RenderDrawLine(renderer, x, y, x+5*rhx, y+5*rhy);
}

void Graphics::draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius) {
    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int err = tx - (radius << 1);

    while (x >= y) {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, center_x + x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y + x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y + x);

        if (err <= 0) {
            y++;
            err += ty;
            ty += 2;
        }

        if (err > 0) {
            x--;
            tx += 2;
            err += tx - (radius << 1);
        }
    }
}

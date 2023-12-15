#include "Thing.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "matlib.h"

#include <iostream>
#include <memory>
#include <vector>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        success = false;
    }

    return success;
}

matrix33 do_2d() {
    float ux = 1;
    float un = -1;
    float xx = 800;
    float xn = 0;
    float vx = 1;
    float vn = -1;
    float yx = 600;
    float yn = 0;
    matrix33 m = TranslateMatrix33(un, vn);
    matrix33 s = ScaleMatrix33((ux-un)/(xx-xn),(vx-vn)/(yx-yn));
    matrix33 t = TranslateMatrix33(-xn, -yn);

    matrix33 mvp = m * s * t;
    //vector3 v = mvp * vector3(400, 300, 1);
    return mvp;
}

SDL_Rect project_rect(SDL_Rect rect, float z, matrix44& mvp) {
    vector4 v1 = mvp * vector4(rect.x, rect.y, z, 1);
    vector4 v2 = mvp * vector4(rect.x + rect.w, rect.y, z, 1);
    vector4 v3 = mvp * vector4(rect.x + rect.w, rect.y + rect.h, z, 1);
    vector4 v4 = mvp * vector4(rect.x, rect.y + rect.h, z, 1);
    v1 = mvp * v1;
    v1 = v1 / v1[3];
    v2 = mvp * v2;
    v2 = v2 / v2[3];
    v3 = mvp * v3;
    v3 = v3 / v3[3];
    v4 = mvp * v4;
    v4 = v4 / v4[3];
    rect.x = v1[0];
    rect.y = v1[1];
    rect.w = v2[0] - v1[0];
    rect.h = v3[1] - v1[1];
    return rect;
}
// TODO figure out: matrix44 p = PerspectiveMatrix44(45, 800.0f/600.0f, 0.1f, 100.0f);
void do_projection_test(SDL_Renderer* renderer) {
    matrix44 pp = IdentityMatrix44();
    pp[2][3] = 1.0f;
    pp[3][3] = 0.0f;
    int radius = 20;
    SDL_Rect rect = {SCREEN_WIDTH/2 - radius, SCREEN_HEIGHT/2 - radius, radius * 2, radius * 2};

    SDL_SetRenderDrawColor(renderer, 0x66, 0xBB, 0x33, 0xFF);
    // SDL_RenderDrawRect(renderer, &rect);
    for (int i = 0; i < 8; ++i) {
        SDL_SetRenderDrawColor(renderer, 0x11 * i, 0xCC, 0x33, 0xFF);
        SDL_Rect new_rect = project_rect(rect, i, pp);
        SDL_RenderDrawRect(renderer, &new_rect);
    }

    SDL_Rect rect2 = {SCREEN_WIDTH/2 - 3*radius, SCREEN_HEIGHT/2 + radius, radius * 2, radius * 2};
    for (int i = 0; i < 8; ++i) {
        SDL_SetRenderDrawColor(renderer, 0xCC, 0x00, 0x18 * i, 0xFF);
        SDL_Rect new_rect = project_rect(rect2, i, pp);
        SDL_RenderDrawRect(renderer, &new_rect);
    }
}

void draw_bezier(SDL_Renderer* renderer, vector2& a, vector2& b, vector2& c, float t) {
    vector2 ab = a + (b-a)*t;
    vector2 bc = b + (c-b)*t;
    vector2 abc = ab + (bc-ab)*t;
    SDL_RenderDrawLine(renderer, abc[0], abc[1], abc[0], abc[1]);
}

void do_quadrant_vision(std::vector<std::unique_ptr<Thing> >& gods, std::vector<std::unique_ptr<Thing> >& monsters) {
    vector4 pos = gods[0]->position();
    vector4 heading = gods[0]->heading();
    vector4 right = gods[0]->right();
    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
        vector4 mpos = (*it)->position();
        vector4 dir = (mpos - pos); //.normalize();
        float dot_h = DotProduct(dir, heading);
        float dot_r = DotProduct(dir, right);
        if (dot_h > 0) {
            // in front
            if (dot_r > 0)
                (*it)->set_side(0);
            else if (dot_r < 0)
                (*it)->set_side(1);
        } else {
            // behind
            if (dot_r > 0)
                (*it)->set_side(3);
            else if (dot_r < 0)
                (*it)->set_side(2);
        }
    }
}

void do_other_vision(std::vector<std::unique_ptr<Thing> >& gods, std::vector<std::unique_ptr<Thing> >& monsters) {
    const float v1 = cos(M_PI / 8);
    const float v2 = cos(M_PI / 4);

    vector4 pos = gods[0]->position();
    vector4 heading = gods[0]->heading();
    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
        vector4 mpos = (*it)->position();
        vector4 dir = (mpos - pos).normalize();
        float dot_h = DotProduct(dir, heading);
        if (dot_h > v1) {
            // in plain sight
            (*it)->set_visibility(1);
        } else if (dot_h > v2) {
            // in peripheral vision
            float visibility = 1 - (v1 - dot_h) / (v1 - v2);
            (*it)->set_visibility(visibility);
        } else {
            // out of sight
            (*it)->set_visibility(0);
        }
    }
}

int main() {
    std::vector<std::unique_ptr<Thing> > gods;
    std::vector<std::unique_ptr<Thing> > monsters;

    gods.push_back(std::unique_ptr<Thing>(new Thing()));
    gods[0]->move(SCREEN_WIDTH/4, SCREEN_HEIGHT/3, 0);
    gods[0]->set_side(-1);

    monsters.push_back(std::unique_ptr<Thing>(new Thing()));
    monsters.push_back(std::unique_ptr<Thing>(new Thing()));
    monsters.push_back(std::unique_ptr<Thing>(new Thing()));
    monsters.push_back(std::unique_ptr<Thing>(new Thing()));
    monsters[0]->move(SCREEN_WIDTH/2, SCREEN_HEIGHT/4, 0);
    monsters[1]->move(SCREEN_WIDTH/3, 2*SCREEN_HEIGHT/3, 0);
    monsters[2]->move(SCREEN_WIDTH/5, SCREEN_HEIGHT/5, 0);
    monsters[3]->move(SCREEN_WIDTH/6, 2*SCREEN_HEIGHT/5, 0);

    vector2 va(100, 100);
    vector2 vb(200, 200);
    vector2 vc(300, 100);
    float t = 0;

    matrix33 m33 = do_2d();

    bool initOk = init();
    if (!initOk) {
        exit(-1);
    }

    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Mob Nerve A", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN);

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
                    for (auto it = gods.begin(); it != gods.end(); ++it) {
                        (*it)->move(3, 0, 0);
                    }
                    for (auto it = monsters.begin(); it != monsters.end(); ++it) {
                        (*it)->move(2, 0, 0);
                    }
                    break;

                case SDLK_DOWN:
                    // TODO rehouse this
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, helloTexture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    break;

                case SDLK_RIGHT:
                    for (auto it = gods.begin(); it != gods.end(); ++it) {
                        (*it)->rotate(10);
                    }
                    break;

                case SDLK_LEFT:
                    for (auto it = gods.begin(); it != gods.end(); ++it) {
                        (*it)->rotate(-10);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        do_quadrant_vision(gods, monsters);
        do_other_vision(gods, monsters);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        do_projection_test(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        // draw_circle(va[0], va[1], 5, renderer);
        // draw_circle(vb[0], vb[1], 5, renderer);
        // draw_circle(vc[0], vc[1], 5, renderer);
        for (float t = 0; t< 1.0; t += 0.01) {
            draw_bezier(renderer, va, vb, vc, t);
        }

        for (auto it = gods.begin(); it != gods.end(); ++it) {
            // TODO only expect one god! (for now...)
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            vector4 pos = (*it)->position();
            vector4 heading = (*it)->heading();
            vector4 right = (*it)->right();
            (*it)->draw(pos, heading, right, renderer);
        }
        for (auto it = monsters.begin(); it != monsters.end(); ++it) {
            if (true) {
                float visibility = (*it)->get_visibility();
                SDL_SetRenderDrawColor(renderer, 0xFF * visibility, 0xFF * visibility, 0xFF * visibility, 0xFF);
            } else {
                int side = (*it)->get_side();
                switch (side) {
                case 0:
                    SDL_SetRenderDrawColor(renderer, 0xCC, 0xCC, 0x00, 0xFF);
                    break;
                case 1:
                    SDL_SetRenderDrawColor(renderer, 0x99, 0x99, 0x99, 0xFF);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderer, 0x00, 0xEE, 0xEE, 0xFF);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderer, 0xEE, 0x00, 0xEE, 0xFF);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    break;
                }
            }
            vector4 pos = (*it)->position();
            vector4 heading = (*it)->heading();
            vector4 right = (*it)->right();
            (*it)->draw(pos, heading, right, renderer);
        }
        SDL_RenderPresent(renderer);

        //SDL_Delay(32);
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

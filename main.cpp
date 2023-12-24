#include "AssetMgr.h"
#include "Mesh.h"
#include "Thing.h"
#include "matlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <memory>
#include <vector>

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 600;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        success = false;
    }

    return success;
}

// page 212 FvD
matrix33 do_alt_2d() {
    float ux = SCREEN_WIDTH - 1;
    float un = 0;
    float xx = 1;
    float xn = -1;
    float vx = 0;
    float vn = SCREEN_HEIGHT - 1;
    float yx = 1;
    float yn = -1;
    matrix33 m = TranslateMatrix33(un, vn);
    matrix33 s = ScaleMatrix33((ux-un)/(xx-xn),(vx-vn)/(yx-yn));
    matrix33 t = TranslateMatrix33(-xn, -yn);

    matrix33 mvp = m * s * t;
    return mvp;
}

void book() {
    // view plane
    vector4 vrp(0, 0, 0, 1);    // view reference point
    vector4 vpn(0, 0, 1, 0);    // view plane normal
    // window on view plane
    float u_min = -50;
    float u_max = 50;
    float v_min = -50;
    float v_max = 50;

    // Viewing Reference Coordinate System
    vector4 vrc_origin = vrp;
    vector3 v_up(0, 1, 0);
    vector3 n_axis;
    n_axis[0] = vpn[0];
    n_axis[1] = vpn[1];
    n_axis[2] = vpn[2];
    vector3 u_axis = CrossProduct(v_up, n_axis);
    // projection
    vector4 prp(0, 0, 1, 1);    // projection reference point
}

void draw_cube(std::vector<vector2>& points, SDL_Renderer* renderer) {
    SDL_RenderDrawLine(renderer, points[0][0], points[0][1], points[1][0], points[1][1]);
    SDL_RenderDrawLine(renderer, points[1][0], points[1][1], points[2][0], points[2][1]);
    SDL_RenderDrawLine(renderer, points[2][0], points[2][1], points[3][0], points[3][1]);
    SDL_RenderDrawLine(renderer, points[3][0], points[3][1], points[0][0], points[0][1]);

    SDL_RenderDrawLine(renderer, points[4][0], points[4][1], points[5][0], points[5][1]);
    SDL_RenderDrawLine(renderer, points[5][0], points[5][1], points[6][0], points[6][1]);
    SDL_RenderDrawLine(renderer, points[6][0], points[6][1], points[7][0], points[7][1]);
    SDL_RenderDrawLine(renderer, points[7][0], points[7][1], points[4][0], points[4][1]);

    SDL_RenderDrawLine(renderer, points[0][0], points[0][1], points[4][0], points[4][1]);
    SDL_RenderDrawLine(renderer, points[1][0], points[1][1], points[5][0], points[5][1]);
    SDL_RenderDrawLine(renderer, points[2][0], points[2][1], points[6][0], points[6][1]);
    SDL_RenderDrawLine(renderer, points[3][0], points[3][1], points[7][0], points[7][1]);
}

// TODO figure out: matrix44 p = PerspectiveMatrix44(45, 800.0f/600.0f, 0.1f, 100.0f);
std::vector<vector2> project_into_screen_space(std::vector<vector4>& points) {
    // project into view plane
    matrix44 pp = IdentityMatrix44();
    pp[2][3] = 1.0f;
    pp[3][3] = 0.0f;
    for (auto it = points.begin(); it != points.end(); ++it) {
        vector4 v = pp * (*it);
        v = v / v[3];
        (*it) = v;
    }

    // move into screen space
    matrix33 w2vp = do_alt_2d();
    std::vector<vector2> view_points;
    vector3 p = vector3(0, 0, 1);
    for (auto it = points.begin(); it != points.end(); ++it) {
        p.x = (*it)[0];
        p.y = (*it)[1];
        p = w2vp * p;
        view_points.push_back(vector2(p.x, p.y));
    }

    return view_points;
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
    AssetMgr assetMgr;
    Mesh cube = assetMgr.loadObj("assets/cube.obj");

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

        static float rot_angle = 0.0f;

        rot_angle += 0.1f;
        rot_angle = fmod(rot_angle, 360.0f);

        {
            matrix44 matTranslate = TranslateMatrix44(0, 0.9, 3);
            matrix44 matRotate = RotateRadMatrix44('y', rot_angle * M_PI / 180.0f);
            matrix44 matTransform = matTranslate * matRotate;
            std::vector<vector4> transformed = cube.getTransformedVertices(matTransform);
            // TODO use vector4 instead of vector3
            std::vector<vector2> vp = project_into_screen_space(transformed);
            SDL_SetRenderDrawColor(renderer, 0xCC, 0x00, 0x10, 0xFF);
            draw_cube(vp, renderer);
        }

        {
            matrix44 matRotate = RotateRadMatrix44('y', rot_angle * M_PI / 180.0f);
            matrix44 matTranslate = TranslateMatrix44(0, 3.2, 3);
            matrix44 matTransform = matTranslate * matRotate;
            std::vector<vector4> transformed = cube.getTransformedVertices(matTransform);
            std::vector<vector2> vp = project_into_screen_space(transformed);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            draw_cube(vp, renderer);
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        // draw_circle(va[0], va[1], 5, renderer);
        // draw_circle(vb[0], vb[1], 5, renderer);
        // draw_circle(vc[0], vc[1], 5, renderer);
        for (float t = 0; t< 1.0; t += 0.01) {
            // draw_bezier(renderer, va, vb, vc, t);
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

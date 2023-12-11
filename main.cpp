#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "matlib.h"

#include <iostream>
#include <memory>
#include <vector>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

void draw_circle(int center_x, int center_y, int radius, SDL_Renderer* renderer) {
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

class Thing {
private:
    matrix44 location;
    int side;
public:
    Thing() {
        location = IdentityMatrix44();
        side = 0;
    }

    ~Thing() {
        std::cout << "Thing destroyed" << std::endl;
    }

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

    void set_side(int s) {
        side = s;
    }

    // void rotate(float x, float y, float z) {
    //     location = location * RotationMatrix44(x, y, z);
    // }

    // void scale(float x, float y, float z) {
    //     location = location * ScaleMatrix44(x, y, z);
    // }

    void draw(SDL_Renderer* renderer) {
        float x = location[3][0];
        float y = location[3][1];
        float hx = location[0][0];
        float hy = location[0][1];
        float rhx = location[1][0];
        float rhy = location[1][1];

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
        draw_circle(x, y, 10, renderer);
        SDL_RenderDrawLine(renderer, x, y, x+10*hx, y+10*hy);
        SDL_RenderDrawLine(renderer, x, y, x+5*rhx, y+5*rhy);
    }
};

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        success = false;
    }

    return success;
}

void do_2d() {
    float ux = 1;
    float un = 0; //-1;
    float xx = 800;
    float xn = 0;
    float vx = 1;
    float vn = 0; //-1;
    float yx = 600;
    float yn = 0;
    matrix33 m = TranslateMatrix33(un, vn);
    matrix33 s = ScaleMatrix33((ux-un)/(xx-xn),(vx-vn)/(yx-yn));
    matrix33 t = TranslateMatrix33(-xn, -yn);

    matrix33 mvp = m * s * t;
    vector3 v = mvp * vector3(400, 300, 1);
}

void project() {
    matrix44 p = PerspectiveMatrix44(45, 800.0f/600.0f, 0.1f, 100.0f);

    matrix44 pp = IdentityMatrix44();
    pp[2][3] = 1.0f;
    pp[3][3] = 0.0f;
    vector4 v = pp * vector4(2, 0, 2, 1);
    vector4 v2 = pp * vector4(20, 0, 20, 1);
}

void x() {

}

// SDL_Rect rect = {center_x - radius, center_y - radius, radius * 2, radius * 2};
// SDL_RenderDrawRect(renderer, &rect);
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
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

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

        vector4 pos = gods[0]->position();
        vector4 heading = gods[0]->heading();
        vector4 right = gods[0]->right();
        for (auto it = monsters.begin(); it != monsters.end(); ++it) {
            vector4 mpos = (*it)->position();
            vector4 dir = (mpos - pos).normalize();
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

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        for (auto it = gods.begin(); it != gods.end(); ++it) {
            (*it)->draw(renderer);
        }
        for (auto it = monsters.begin(); it != monsters.end(); ++it) {
            (*it)->draw(renderer);
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

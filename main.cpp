#include "AssetMgr.h"
#include "Mesh.h"
#include "Thing.h"
#include "matlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <memory>
#include <vector>

// #define INDEX_FORWARD(Index) \
//     Index = (Index + 1) % verts.size();
#define INDEX_BACKWARD(Index) \
    Index = (Index - 1 + verts.size()) % verts.size();
#define INDEX_MOVE(Index,Direction) \
    if (Direction > 0) \
        Index = (Index + 1) % verts.size(); \
    else \
        Index = (Index - 1 + verts.size()) % verts.size();

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

struct HLine {
    int XStart;     // left-most pixel in the line
    int XEnd;       // right-...
};

struct HLineList {
    int Length;
    int YStart;
    struct HLine *HLinePtr;
};

void ScanEdge(int X1, int Y1, int X2, int Y2, int SetXStart,
    int SkipFirst, struct HLine **EdgePointPtr)
{
    int Y, DeltaX, DeltaY;
    double InverseSlope;
    struct HLine *WorkingEdgePointPtr;

    // calculate X and Y lengths of the line and the inverse slope
    DeltaX = X2 - X1;
    if ((DeltaY = Y2 - Y1) <= 0)
        return;     // guard against 0 length and horizontal edges
    InverseSlope = (double)DeltaX / (double)DeltaY;
    // store the x coordinate of the pixel closest to but not to the
    // left of the line for each Y coorditane between Y1 and Y2, not
    // including Y2 and also not including Y1 if SkipFirst != 0
    WorkingEdgePointPtr = *EdgePointPtr;
    for (Y = Y1 + SkipFirst; Y < Y2; Y++, WorkingEdgePointPtr++) {
        // store the x coordinate in the appropriate edge list
        if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1 + (int)(ceil((Y-Y1) * InverseSlope));
        else
            WorkingEdgePointPtr->XEnd = X1 + (int)(ceil((Y-Y1) * InverseSlope));
    }
    // advance caller's pointer
    *EdgePointPtr = WorkingEdgePointPtr;
}

void DrawHorizontalLineList(SDL_Renderer* renderer, struct HLineList *HLineListPtr,
      int Color)
{
    // point to the XStart/XEnd descriptor for the first (top)
    // horizontal line
    struct HLine *HLinePtr = HLineListPtr->HLinePtr;
    // draw each horizontal line in turn, starting with the top one and
    // advancing one line each time
    for (int Y = HLineListPtr->YStart; Y < (HLineListPtr->YStart +
          HLineListPtr->Length); Y++, HLinePtr++) {
        // draw each pixel in the current horizontal line in turn,
        // starting with the leftmost one
        SDL_RenderDrawLine(renderer, HLinePtr->XStart, Y, HLinePtr->XEnd, Y);
    }
}

int fill_polygon(SDL_Renderer* renderer, std::vector<vector2>& verts, int Color, int XOffset, int YOffset) {
    int MinIndexL, MaxIndex, MinIndexR, SkipFirst;
    int MinPoint_Y, MaxPoint_Y, TopIsFlat, LeftEdgeDir;
    int CurrentIndex, PreviousIndex;
    int DeltaXN, DeltaYN, DeltaXP, DeltaYP;
    struct HLineList WorkingLineList;
    struct HLine *EdgePointPtr;

    if (verts.size() == 0) {
        return 1;
    }

    MaxPoint_Y = MinPoint_Y = verts[MinIndexL = MaxIndex = 0].y;
    for (int i = 1; i < verts.size(); i++) {
        if (verts[i].y < MinPoint_Y)
            MinPoint_Y = verts[MinIndexL = i].y;    // new top
        else if (verts[i].y > MaxPoint_Y)
            MaxPoint_Y = verts[MaxIndex = i].y;     // new bottom
    }
    if (MinPoint_Y == MaxPoint_Y)
        return 1;   // polygon is 0 height
    // scan in ascending order to find last top edge point
    MinIndexR = MinIndexL;
    while (verts[MinIndexR].y == MinPoint_Y)
        MinIndexR = (MinIndexR + 1) % verts.size();
    INDEX_BACKWARD(MinIndexR);      // back up to last top edge point
    // now scan in descending order to find first top edge point
    while (verts[MinIndexL].y == MinPoint_Y)
        INDEX_BACKWARD(MinIndexL);
    // back up to first top edge point
    MinIndexL = (MinIndexL + 1) % verts.size();
    // ?? figure out which direction through the verts from the top;
    // vertex is the left edge and which is the right
    LeftEdgeDir = -1;   // assume left edge runs down thru verts
    TopIsFlat = (verts[MinIndexL].x != verts[MinIndexR].x) ? 1 : 0;
    if (TopIsFlat == 1) {
        // if the top is flat, just see which of the ends is leftmost
        if (verts[MinIndexL].x > verts[MinIndexR].x) {
            LeftEdgeDir = 1;        // TODO fill in comments
            int Temp = MinIndexL;       //
            MinIndexL = MinIndexR;  //
            MinIndexR = Temp;       // etc
        }
    } else {
        // point to the downward end of the first line ...
        int NextIndex = MinIndexR;
        NextIndex = (NextIndex + 1) % verts.size();

        PreviousIndex = MinIndexL;
        INDEX_BACKWARD(PreviousIndex);
        // calculate X and Y lengths from the top vertex to the end of
        // the ...
        //
        DeltaXN = verts[NextIndex].x - verts[MinIndexL].x;
        DeltaYN = verts[NextIndex].y - verts[MinIndexL].y;
        DeltaXP = verts[PreviousIndex].x - verts[MinIndexL].x;
        DeltaYP = verts[PreviousIndex].y - verts[MinIndexL].y;
        if (((long)DeltaXN * DeltaYP - (long)DeltaYN * DeltaXP) < 0L) {
            LeftEdgeDir = 1;        // TODO ...
            int Temp = MinIndexL;
            MinIndexL = MinIndexR;
            MinIndexR = Temp;
        }
    }

    // set the # of scan lines in the polygon, skipping the bottom edge
    // and ...
    // in ...
    // the ...
    // the ...
    WorkingLineList.Length = MaxPoint_Y - MinPoint_Y - 1 + TopIsFlat;
    if (WorkingLineList.Length <= 0)
        return 1;
    WorkingLineList.YStart = YOffset + MinPoint_Y + 1 - TopIsFlat;
    // get memory
    if ((WorkingLineList.HLinePtr =
          (struct HLine *) (malloc(sizeof(struct HLine) *
          WorkingLineList.Length))) == NULL)
        return 0;
    // scan the left ege and store the boundary points in the list
    // initial pointer for storing scan converted left-edge coords
    EdgePointPtr = WorkingLineList.HLinePtr;
    // start from the top of the left edge
    PreviousIndex = CurrentIndex = MinIndexL;
    // skip the first point of the first line unless the top is flat
    // if the top isn't flat, the top vertex is exatly on a right edge
    // and isn't drawn
    SkipFirst = TopIsFlat ? 0 : 1;
    // scan convert each line in the left edge from top to bottom
    do {
        if (LeftEdgeDir > 0)
            CurrentIndex = (CurrentIndex + 1) % verts.size();
        else
            CurrentIndex = (CurrentIndex - 1 + verts.size()) % verts.size();
        ScanEdge(verts[PreviousIndex].x + XOffset,
            verts[PreviousIndex].y,
            verts[CurrentIndex].x + XOffset,
            verts[CurrentIndex].y, 1, SkipFirst, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
        SkipFirst = 0;      // scan convert the first point from now on
    } while (CurrentIndex != MaxIndex);

    // scan the right edge and store the boundary points in the list
    EdgePointPtr = WorkingLineList.HLinePtr;
    PreviousIndex = CurrentIndex = MinIndexR;
    SkipFirst = TopIsFlat ? 0 : 1;
    // scan convert the right edge, top to bottom. X coordinates are
    // adjusted 1 to the left, effectively causing scan conversion of
    // the nearest points to the left of but not exactly on the edge
    do {
        INDEX_MOVE(CurrentIndex, -LeftEdgeDir);
        ScanEdge(verts[PreviousIndex].x + XOffset - 1,
            verts[PreviousIndex].y,
            verts[CurrentIndex].x + XOffset - 1,
            verts[CurrentIndex].y, 0, SkipFirst, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
        SkipFirst = 0;
    } while (CurrentIndex != MaxIndex);

    // draw the line list representing the scan converted polygon
    DrawHorizontalLineList(renderer, &WorkingLineList, Color);

    // release the line list's memory
    free(WorkingLineList.HLinePtr);
    return 0;
}

void draw_triangles(SDL_Renderer* renderer, std::vector<vector2>& points, const std::vector<Triangle>& triangles) {
    for (int i =  0; i < triangles.size(); i++) {
        const Triangle& t = triangles[i];

        bool fill_polys = true;
        if (fill_polys) {
            SDL_SetRenderDrawColor(renderer, 0x99, 0x99, 0x99, 0xFF);
            std::vector<vector2> verts;
            verts.push_back(points[t.v1]);
            verts.push_back(points[t.v2]);
            verts.push_back(points[t.v3]);
            fill_polygon(renderer, verts, 0, 0, 0);
        }

        bool draw_outlines = true;
        if (draw_outlines) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xCC, 0xFF);
            SDL_RenderDrawLine(renderer, points[t.v1][0], points[t.v1][1], points[t.v2][0], points[t.v2][1]);
            SDL_RenderDrawLine(renderer, points[t.v2][0], points[t.v2][1], points[t.v3][0], points[t.v3][1]);
            SDL_RenderDrawLine(renderer, points[t.v3][0], points[t.v3][1], points[t.v1][0], points[t.v1][1]);
        }
    }
}

// TODO figure out: matrix44 p = PerspectiveMatrix44(45, 800.0f/600.0f, 0.1f, 100.0f);
std::vector<vector2> project_into_screen_space(std::vector<vector4>& points) {
    // project into view plane
    matrix44 pp = IdentityMatrix44();
    pp[2][3] = 1.0f;
    pp[3][3] = 0.0f;
    for (auto it = points.begin(); it != points.end(); it++) {
        vector4 v = pp * (*it);
        v = v / v[3];
        (*it) = v;
    }

    // move into screen space
    matrix33 w2vp = do_alt_2d();
    std::vector<vector2> view_points;
    vector3 p = vector3(0, 0, 1);
    for (auto it = points.begin(); it != points.end(); it++) {
        p.x = (*it)[0];
        p.y = (*it)[1];
        p = w2vp * p;
        view_points.push_back(vector2((int)p.x, (int)p.y));
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
    vector4 pos = gods[0]->get_location().get_position();
    vector4 heading = gods[0]->heading();
    vector4 right = gods[0]->right();
    for (auto it = monsters.begin(); it != monsters.end(); it++) {
        vector4 mpos = (*it)->get_location().get_position();
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

    vector4 pos = gods[0]->get_location().get_position();
    vector4 heading = gods[0]->heading();
    for (auto it = monsters.begin(); it != monsters.end(); it++) {
        vector4 mpos = (*it)->get_location().get_position();
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
    int frame_no = 0;

    AssetMgr assetMgr;
    Mesh cube = assetMgr.loadObj("assets/cube.obj");
    Mesh ico1 = assetMgr.loadObj("assets/ico1.obj");
    Mesh ico4 = assetMgr.loadObj("assets/ico4.obj");

    std::vector<std::unique_ptr<Thing>> gods;
    std::vector<std::unique_ptr<Thing>> monsters;

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

    std::vector<std::unique_ptr<Thing>> objects;
    objects.push_back(std::unique_ptr<Thing>(new Thing()));
    objects.push_back(std::unique_ptr<Thing>(new Thing()));
    objects.push_back(std::unique_ptr<Thing>(new Thing()));
    std::shared_ptr<Mesh> ico4_ptr = std::make_shared<Mesh>(ico4);
    std::shared_ptr<Mesh> ico1_ptr = std::make_shared<Mesh>(ico1);
    std::shared_ptr<Mesh> cube_ptr = std::make_shared<Mesh>(cube);
    objects[0]->move(0, -3.1, 29);
    objects[0]->get_graphics().set_mesh(cube_ptr);
    objects[1]->move(0, -3.1, 12);
    objects[1]->get_graphics().set_mesh(ico1_ptr);
    objects[2]->move(0, -3.1, 6);
    objects[2]->get_graphics().set_mesh(ico4_ptr);

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
        ++frame_no;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    for (auto it = gods.begin(); it != gods.end(); it++) {
                        vector4 heading = (*it)->heading();
                        (*it)->move(3*heading.x, 3*heading.y, 3*heading.z);
                    }
                    for (auto it = monsters.begin(); it != monsters.end(); it++) {
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
                    for (auto it = gods.begin(); it != gods.end(); it++) {
                        (*it)->rotate('z', 10);
                    }
                    break;

                case SDLK_LEFT:
                    for (auto it = gods.begin(); it != gods.end(); it++) {
                        (*it)->rotate('z', -10);
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

        {
            // do stuff
            objects[0]->rotate('y', 0.5f);
            objects[1]->rotate('y', -0.7f);
            objects[2]->rotate('z', -0.45f);

            // vector4 vpn{0, 0, 1, 0};
            vector4 camera{0, 0, 0, 1};
            for (auto it = objects.begin(); it != objects.end(); it++) {
                // THING STATE
                std::shared_ptr<Mesh> mesh = (*it)->get_graphics().get_mesh();
                if (mesh == nullptr) {
                    continue;
                }
                matrix44 transform = (*it)->get_location().get_transform();
                std::vector<vector4> transformed = mesh->get_transformed_vertices(transform);
                std::vector<vector4> normals = mesh->get_transformed_vertex_normals(transform);
                std::vector<Triangle> triangles = mesh->get_triangles();
                // MESH
                std::vector<vector2> ssv = project_into_screen_space(transformed);
                triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&camera, &normals, &transformed](const Triangle& t) {
                    vector4 from_camera = (transformed[t.v1] - camera).normalize();
                    float dot = DotProduct(from_camera, normals[t.vn1]);
                    return dot > 0;
                }), triangles.end());
                SDL_SetRenderDrawColor(renderer, 0xCC, 0x00, 0x10, 0xFF);
                draw_triangles(renderer, ssv, triangles);
                // NORMALS
                bool draw_normals = true;
                if (draw_normals) {
                    std::vector<vector4> tnv;
                    float scale = 0.025;
                    for (auto it = triangles.begin(); it != triangles.end(); it++) {
                        tnv.push_back(transformed[it->v1]);
                        tnv.push_back(transformed[it->v1] + normals[it->vn1] * scale);
                        tnv.push_back(transformed[it->v2]);
                        tnv.push_back(transformed[it->v2] + normals[it->vn2] * scale);
                        tnv.push_back(transformed[it->v3]);
                        tnv.push_back(transformed[it->v3] + normals[it->vn3] * scale);
                    }
                    std::vector<vector2> ssn = project_into_screen_space(tnv);
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0x99, 0xFF, 0xFF);
                    for (int i = 0; i < ssn.size(); i += 2) {
                        SDL_RenderDrawLine(renderer, ssn[i][0], ssn[i][1], ssn[i+1][0], ssn[i+1][1]);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        // draw_circle(va[0], va[1], 5, renderer);
        // draw_circle(vb[0], vb[1], 5, renderer);
        // draw_circle(vc[0], vc[1], 5, renderer);
        for (float t = 0; t< 1.0; t += 0.01) {
            // draw_bezier(renderer, va, vb, vc, t);
        }

        for (auto it = gods.begin(); it != gods.end(); it++) {
            // TODO only expect one god! (for now...)
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            vector4 pos = (*it)->get_location().get_position();
            vector4 heading = (*it)->heading();
            vector4 right = (*it)->right();
            (*it)->draw(pos, heading, right, renderer);
        }
        for (auto it = monsters.begin(); it != monsters.end(); it++) {
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
            vector4 pos = (*it)->get_location().get_position();
            vector4 heading = (*it)->heading();
            vector4 right = (*it)->right();
            (*it)->draw(pos, heading, right, renderer);
        }
        SDL_RenderPresent(renderer);

        // 60 fps the crummy way
        SDL_Delay(16);
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

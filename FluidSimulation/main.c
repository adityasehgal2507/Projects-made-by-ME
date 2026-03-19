#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define GRID_W 100
#define GRID_H 100
#define CELL_SIZE 6
#define INJECT_AMOUNT 1.0f

#define DIFFUSION 0.001f
#define DISSIPATION .999f  // 1.0 = no decay, lower = faster decay

typedef struct {
    Vector2 vel;
    float density;
} Cell;

int walls[GRID_W * GRID_H];

int vecToId(int x, int y) {
    if (x < 0)
        x = 0;
    if (x >= GRID_W)
        x = GRID_W - 1;
    if (y < 0)
        y = 0;
    if (y >= GRID_H)
        y = GRID_H - 1;
    return y * GRID_W + x;
}

void idToVec(int id, int* x, int* y) {
    *x = id % GRID_W;
    *y = id / GRID_W;
}

void Draw_Grid(Cell* grid) {
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            int i = vecToId(x, y);

            if (walls[i]) {
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
                continue;
            }

            float d = grid[i].density;
            if (d > 1.0f)
                d = 1.0f;
            if (d < 0.0f)
                d = 0.0f;

            Color c = (Color){
                (unsigned char)(d * 255),
                (unsigned char)(d * 255),
                (unsigned char)(d * 255),
                255};

            DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, c);
        }
    }
}

void Draw_Vector(Cell* grid) {
    for (int y = 0; y < GRID_H; y += 4) {
        for (int x = 0; x < GRID_W; x += 4) {
            int id = vecToId(x, y);
            if (walls[id])
                continue;
            Vector2 start = {x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2};
            Vector2 end = Vector2Add(start, Vector2Scale(grid[id].vel, 10.0f));
            DrawLineV(start, end, RED);
            DrawCircleV(end, 1.5f, RED);
        }
    }
}

void Inject(Cell* grid) {
    static Vector2 prev_mouse = {0};

    Vector2 mouse = GetMousePosition();
    Vector2 delta = Vector2Subtract(mouse, prev_mouse);

    int cx = (int)(mouse.x / CELL_SIZE);
    int cy = (int)(mouse.y / CELL_SIZE);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        for (int y = cy - 2; y <= cy + 2; y++) {
            for (int x = cx - 2; x <= cx + 2; x++) {
                if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H)
                    continue;
                int id = vecToId(x, y);
                if (walls[id])
                    continue;

                grid[id].vel = Vector2Add(grid[id].vel, Vector2Scale(delta, 0.5f));
                grid[id].density += INJECT_AMOUNT;
                if (grid[id].density > 1.0f)
                    grid[id].density = 1.0f;
            }
        }
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        for (int y = cy - 1; y <= cy + 1; y++) {
            for (int x = cx - 1; x <= cx + 1; x++) {
                if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H)
                    continue;
                int id = vecToId(x, y);
                walls[id] = 1;
                grid[id].vel = (Vector2){0, 0};
                grid[id].density = 0.0f;
            }
        }
    }

    // Middle click to erase walls
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
        for (int y = cy - 1; y <= cy + 1; y++) {
            for (int x = cx - 1; x <= cx + 1; x++) {
                if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H)
                    continue;
                walls[vecToId(x, y)] = 0;
            }
        }
    }

    prev_mouse = mouse;
}

void Diffuse(Cell* grid, Cell* new_grid, float dt) {
    memcpy(new_grid, grid, GRID_W * GRID_H * sizeof(Cell));

    float a = dt * DIFFUSION * GRID_W * GRID_H;

    for (int k = 0; k < 20; k++) {
        for (int y = 0; y < GRID_H; y++) {
            for (int x = 0; x < GRID_W; x++) {
                int id = vecToId(x, y);
                if (walls[id])
                    continue;

                int xm = max(x - 1, 0);
                int xp = min(x + 1, GRID_W - 1);
                int ym = max(y - 1, 0);
                int yp = min(y + 1, GRID_H - 1);

                Cell MIDDLE = grid[id];

                // treat wall neighbors as zero
                Cell UP = walls[vecToId(x, ym)] ? new_grid[id] : new_grid[vecToId(x, ym)];
                Cell DOWN = walls[vecToId(x, yp)] ? new_grid[id] : new_grid[vecToId(x, yp)];
                Cell LEFT = walls[vecToId(xm, y)] ? new_grid[id] : new_grid[vecToId(xm, y)];
                Cell RIGHT = walls[vecToId(xp, y)] ? new_grid[id] : new_grid[vecToId(xp, y)];

                float density_sum = UP.density + DOWN.density + LEFT.density + RIGHT.density;
                new_grid[id].density = (MIDDLE.density + a * density_sum) / (1 + 4 * a);

                Vector2 vel_sum = Vector2Add(
                    Vector2Add(UP.vel, DOWN.vel),
                    Vector2Add(LEFT.vel, RIGHT.vel));
                new_grid[id].vel = Vector2Scale(
                    Vector2Add(MIDDLE.vel, Vector2Scale(vel_sum, a)),
                    1.0f / (1 + 4 * a));
            }
        }
    }

    memcpy(grid, new_grid, GRID_W * GRID_H * sizeof(Cell));
}

void Advect(Cell* grid, Cell* new_grid, float dt) {
    memcpy(new_grid, grid, GRID_W * GRID_H * sizeof(Cell));

    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            int id = vecToId(x, y);
            if (walls[id])
                continue;

            Cell MIDDLE = grid[id];

            Vector2 prev = Vector2Subtract(
                (Vector2){(float)x, (float)y},
                Vector2Scale(MIDDLE.vel, dt));

            Vector2 prev_clamped = Vector2Clamp(
                prev,
                (Vector2){0.5f, 0.5f},
                (Vector2){(float)GRID_W - 1.5f, (float)GRID_H - 1.5f});

            int x0 = (int)prev_clamped.x;
            int y0 = (int)prev_clamped.y;
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            float s1 = prev_clamped.x - x0;
            float s0 = 1.0f - s1;
            float t1 = prev_clamped.y - y0;
            float t0 = 1.0f - t1;

            Cell BL = walls[vecToId(x0, y0)] ? (Cell){0} : grid[vecToId(x0, y0)];
            Cell BR = walls[vecToId(x1, y0)] ? (Cell){0} : grid[vecToId(x1, y0)];
            Cell TL = walls[vecToId(x0, y1)] ? (Cell){0} : grid[vecToId(x0, y1)];
            Cell TR = walls[vecToId(x1, y1)] ? (Cell){0} : grid[vecToId(x1, y1)];

            new_grid[id].density =
                (s0 * (t0 * BL.density + t1 * TL.density) +
                 s1 * (t0 * BR.density + t1 * TR.density)) *
                DISSIPATION;

            new_grid[id].vel = (Vector2){
                s0 * (t0 * BL.vel.x + t1 * TL.vel.x) + s1 * (t0 * BR.vel.x + t1 * TR.vel.x),
                s0 * (t0 * BL.vel.y + t1 * TL.vel.y) + s1 * (t0 * BR.vel.y + t1 * TR.vel.y)};
        }
    }

    memcpy(grid, new_grid, GRID_W * GRID_H * sizeof(Cell));
}

void Project(Cell* grid, Cell* temp) {
    float* div = (float*)calloc(GRID_W * GRID_H, sizeof(float));
    float* p = (float*)calloc(GRID_W * GRID_H, sizeof(float));

    for (int y = 1; y < GRID_H - 1; y++) {
        for (int x = 1; x < GRID_W - 1; x++) {
            int id = vecToId(x, y);
            if (walls[id])
                continue;

            float vx_right = walls[vecToId(x + 1, y)] ? 0 : grid[vecToId(x + 1, y)].vel.x;
            float vx_left = walls[vecToId(x - 1, y)] ? 0 : grid[vecToId(x - 1, y)].vel.x;
            float vy_up = walls[vecToId(x, y + 1)] ? 0 : grid[vecToId(x, y + 1)].vel.y;
            float vy_down = walls[vecToId(x, y - 1)] ? 0 : grid[vecToId(x, y - 1)].vel.y;

            div[id] = 0.5f * ((vx_right - vx_left) + (vy_up - vy_down));
            p[id] = 0.0f;
        }
    }

    for (int k = 0; k < 20; k++) {
        for (int y = 1; y < GRID_H - 1; y++) {
            for (int x = 1; x < GRID_W - 1; x++) {
                int id = vecToId(x, y);
                if (walls[id])
                    continue;

                float pL = walls[vecToId(x - 1, y)] ? p[id] : p[vecToId(x - 1, y)];
                float pR = walls[vecToId(x + 1, y)] ? p[id] : p[vecToId(x + 1, y)];
                float pD = walls[vecToId(x, y - 1)] ? p[id] : p[vecToId(x, y - 1)];
                float pU = walls[vecToId(x, y + 1)] ? p[id] : p[vecToId(x, y + 1)];

                p[id] = (pL + pR + pD + pU - div[id]) / 4.0f;
            }
        }
    }

    for (int y = 1; y < GRID_H - 1; y++) {
        for (int x = 1; x < GRID_W - 1; x++) {
            int id = vecToId(x, y);
            if (walls[id])
                continue;

            float pL = walls[vecToId(x - 1, y)] ? p[id] : p[vecToId(x - 1, y)];
            float pR = walls[vecToId(x + 1, y)] ? p[id] : p[vecToId(x + 1, y)];
            float pD = walls[vecToId(x, y - 1)] ? p[id] : p[vecToId(x, y - 1)];
            float pU = walls[vecToId(x, y + 1)] ? p[id] : p[vecToId(x, y + 1)];

            grid[id].vel.x -= 0.5f * (pR - pL);
            grid[id].vel.y -= 0.5f * (pU - pD);
        }
    }

    // Border and wall velocity clamp
    for (int x = 0; x < GRID_W; x++) {
        grid[vecToId(x, 0)].vel = (Vector2){0, 0};
        grid[vecToId(x, GRID_H - 1)].vel = (Vector2){0, 0};
    }
    for (int y = 0; y < GRID_H; y++) {
        grid[vecToId(0, y)].vel = (Vector2){0, 0};
        grid[vecToId(GRID_W - 1, y)].vel = (Vector2){0, 0};
    }

    free(div);
    free(p);
}

int main() {
    memset(walls, 0, sizeof(walls));

    InitWindow(GRID_W * CELL_SIZE, GRID_H * CELL_SIZE, "Fluid Sim");
    SetTargetFPS(60);

    Cell* grid = (Cell*)calloc(GRID_W * GRID_H, sizeof(Cell));
    Cell* temp = (Cell*)calloc(GRID_W * GRID_H, sizeof(Cell));

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        Inject(grid);
        Diffuse(grid, temp, dt);
        Advect(grid, temp, dt);
        Project(grid, temp);

        BeginDrawing();
        ClearBackground(BLACK);
        Draw_Grid(grid);
        Draw_Vector(grid);
        EndDrawing();
    }

    free(grid);
    free(temp);
    CloseWindow();
    return 0;
}
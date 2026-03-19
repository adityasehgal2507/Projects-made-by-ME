#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#define MAX_BOIDS 500
#define BOID_SIZE 10
#define RADIUS 50
#define SEP_RADIUS 25
#define MAX_SPEED 100

#define ALI_W 1.4f
#define COH_W 0.3f
#define SEP_W 1.0f

typedef struct Boid {
    Vector2 Position;
    Vector2 Velocity;
} Boid;

void DrawCustomTriangle(Vector2 center, float rotation) {
    DrawPoly(center, 3, BOID_SIZE, rotation, WHITE);
}

void DrawFrame(Boid* boids) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < MAX_BOIDS; i++) {
        Boid b = boids[i];
        float angle = atan2(b.Velocity.y, b.Velocity.x) * RAD2DEG;
        DrawCustomTriangle(b.Position, angle);
    }

    EndDrawing();
}

Boid* getNeighbours(Boid* b, Boid* all, int* outCount) {
    static Boid neighbours[MAX_BOIDS];  // temporary array
    int count = 0;

    for (int i = 0; i < MAX_BOIDS; i++) {
        if (&all[i] == b)
            continue;  // skip itself

        float dx = b->Position.x - all[i].Position.x;
        float dy = b->Position.y - all[i].Position.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < RADIUS) {
            neighbours[count++] = all[i];
        }
    }

    *outCount = count;
    return neighbours;
}
void Seperate(Boid* all) {
    for (int i = 0; i < MAX_BOIDS; i++) {
        Vector2 sep_sum = {0, 0};
        int neighbour_count;
        Boid* neighbours = getNeighbours(&all[i], all, &neighbour_count);

        for (int j = 0; j < neighbour_count; j++) {
            float dist = Vector2Distance(all[i].Position, neighbours[j].Position);
            if (dist > SEP_RADIUS)
                continue;

            Vector2 diff = Vector2Subtract(all[i].Position, neighbours[j].Position);
            float distSqr = fmax(Vector2DistanceSqr(all[i].Position, neighbours[j].Position), 0.01f);
            sep_sum = Vector2Add(sep_sum, Vector2Scale(diff, 1.0f / distSqr));
        }

        all[i].Velocity = Vector2Add(all[i].Velocity, Vector2Scale(sep_sum, SEP_W));
    }
}

void Align(Boid* all) {
    for (int i = 0; i < MAX_BOIDS; i++) {
        Vector2 ali_sum = {0, 0};
        int neighbour_count;
        Boid* neighbours = getNeighbours(&all[i], all, &neighbour_count);

        if (neighbour_count > 0) {
            for (int j = 0; j < neighbour_count; j++) {
                Vector2 diff = Vector2Subtract(neighbours[j].Velocity, all[i].Velocity);
                float len = Vector2Length(diff);
                if (len > 0)
                    diff = Vector2Scale(diff, 1.0f / len);
                ali_sum = Vector2Add(ali_sum, diff);
            }
            ali_sum = Vector2Scale(ali_sum, 1.0f / neighbour_count);
            all[i].Velocity = Vector2Add(all[i].Velocity, Vector2Scale(ali_sum, ALI_W));
        }
    }
}

void Cohesion(Boid* all) {
    for (int i = 0; i < MAX_BOIDS; i++) {
        Vector2 coh_sum = {0, 0};
        int neighbour_count;
        Boid* neighbours = getNeighbours(&all[i], all, &neighbour_count);

        if (neighbour_count > 0) {
            for (int j = 0; j < neighbour_count; j++)
                coh_sum = Vector2Add(coh_sum, neighbours[j].Position);

            Vector2 center = Vector2Scale(coh_sum, 1.0f / neighbour_count);
            Vector2 moveToward = Vector2Subtract(center, all[i].Position);

            // normalize so distance doesn't matter, just direction
            float len = Vector2Length(moveToward);
            if (len > 0)
                moveToward = Vector2Scale(moveToward, 1.0f / len);

            all[i].Velocity = Vector2Add(all[i].Velocity, Vector2Scale(moveToward, COH_W));
        }
    }
}

void LimitSpeed(Boid* all) {
    for (int i = 0; i < MAX_BOIDS; i++) {
        float speed = Vector2Length(all[i].Velocity);
        if (speed > MAX_SPEED) {
            all[i].Velocity = Vector2Scale(all[i].Velocity, MAX_SPEED / speed);
        }
    }
}

void SetBounds(Boid* all) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (int i = 0; i < MAX_BOIDS; i++) {
        if (all[i].Position.x > width) {
            all[i].Position.x = 0;
        } else if (all[i].Position.x < 0) {
            all[i].Position.x = width;
        }

        if (all[i].Position.y > height) {
            all[i].Position.y = 0;
        } else if (all[i].Position.y < 0) {
            all[i].Position.y = height;
        }
    }
}

Boid* init_boids() {
    Boid* boids = (Boid*)calloc(MAX_BOIDS, sizeof(Boid));
    if (!boids)
        return NULL;  // safety check

    for (int i = 0; i < MAX_BOIDS; i++) {
        boids[i].Position = (Vector2){
            GetRandomValue(0, GetScreenWidth()),
            GetRandomValue(0, GetScreenHeight())};

        boids[i].Velocity = (Vector2){
            GetRandomValue(-MAX_SPEED, MAX_SPEED) / .5f,
            GetRandomValue(-MAX_SPEED, MAX_SPEED) / .5f};
    }

    return boids;
}

void UpdatePosition(Boid* all) {
    float dt = GetFrameTime();
    for (int i = 0; i < MAX_BOIDS; i++) {
        all[i].Position = Vector2Add(all[i].Position, Vector2Scale(all[i].Velocity, dt));
    }
}

int main(int argc, char const* argv[]) {
    // Init window
    InitWindow(800, 600, "Boids Simulation");
    SetTargetFPS(60);

    // Init boids
    Boid* boids = init_boids();
    if (!boids)
        return 1;

    // Main loop
    while (!WindowShouldClose()) {
        Align(boids);
        Cohesion(boids);
        Seperate(boids);
        LimitSpeed(boids);
        UpdatePosition(boids);
        SetBounds(boids);
        DrawFrame(boids);
    }

    // Cleanup
    free(boids);
    CloseWindow();

    return 0;
}
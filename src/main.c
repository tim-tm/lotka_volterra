#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct _Tick_ {
    float prey;
    float predators;
} Tick;

float alpha = 0.2f;   // Prey birth rate
float beta = 0.02f;   // Predation rate
float delta = 0.01f;  // Predator reproduction rate
float gammma = 0.1f;  // Predator death rate
float scale = 7.f;

Tick current_tick = (Tick){40.f, 9.f};
size_t ticks_size = 256;
size_t ticks_len = 0;
Tick *ticks;

void update_populations(float dt) {
    // https://en.wikipedia.org/wiki/Lotka%E2%80%93Volterra_equations
    float preyGrowth = (alpha * current_tick.prey - beta * current_tick.prey * current_tick.predators) * dt;
    float predatorGrowth = (delta * current_tick.prey * current_tick.predators - gammma * current_tick.predators) * dt;

    current_tick.prey += preyGrowth;
    current_tick.predators += predatorGrowth;
    if (current_tick.prey < 0) current_tick.prey = 0;
    if (current_tick.predators < 0) current_tick.predators = 0;

    ticks[ticks_len++] = current_tick;
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    ticks = calloc(ticks_size, sizeof(Tick));
    if (ticks == NULL) {
        fprintf(stderr, "calloc: No mem.\n");
        return 1;
    }

    InitWindow(screenWidth, screenHeight, "Lotka-Volterra Simulation");
    SetTargetFPS(60);

    float camX = 0.f;
    float camY = 0.f;
    float camSlider = 0.f;

    Camera2D camera = {0};
    camera.offset = (Vector2){0, 0};
    camera.zoom = 1.f;
    camera.rotation = 0.f;
    camera.target = (Vector2){camX, camY};

    while (!WindowShouldClose()) {
        if (ticks_len > ticks_size) {
            ticks_size *= 2;
            ticks = realloc(ticks, ticks_size*sizeof(Tick));
            if (ticks == NULL) {
                fprintf(stderr, "realloc: No mem.\n");
                return 1;
            }
        }
        if (ticks_len > screenWidth-300) {
            if (ticks_len > screenWidth-300+camX) {
                camX += screenWidth/2.f;
            }
        }
        camera.target = (Vector2){camSlider, camY};
        
        update_populations(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        for (int i = 0; i < ticks_len-1; i++) {
            int yoff1 = (int)fmax(0, fmin(ticks[i].prey*scale, screenHeight));
            int yoff2 = (int)fmax(0, fmin(ticks[i].predators*scale, screenHeight));   
            int yoff3 = (int)fmax(0, fmin(ticks[i+1].prey*scale, screenHeight));
            int yoff4 = (int)fmax(0, fmin(ticks[i+1].predators*scale, screenHeight));

            DrawLineEx((Vector2){5+i, screenHeight-5-yoff1}, (Vector2){6+i, screenHeight-5-yoff3}, 1.5f, GREEN);
            DrawLineEx((Vector2){5+i, screenHeight-5-yoff2}, (Vector2){6+i, screenHeight-5-yoff4}, 1.5f, RED);
        }
        EndMode2D();
        GuiSlider((Rectangle){0, 0, screenWidth-500, 20}, NULL, "scroll", &camSlider, 0.f, camX);

        DrawRectangle(screenWidth-300, 0, screenWidth-500, screenHeight, LIGHTGRAY);      
        DrawText("Lotka-Volterra Simulation", screenWidth-280, 10, 20, DARKGRAY);
        DrawText(TextFormat("Beute: %.2f", current_tick.prey), screenWidth-220, 40, 20, GREEN);
        DrawText(TextFormat("Räuber: %.2f", current_tick.predators), screenWidth-220, 70, 20, RED);
        DrawText(TextFormat("ticks_size: %zu", ticks_size), screenWidth-220, 100, 20, DARKGRAY);
        GuiSliderBar((Rectangle){screenWidth-170, 140, 120, 20}, "Geburtenrate: Beute", TextFormat("%.2f", alpha), &alpha, 0.01f, 1.f);
        GuiSliderBar((Rectangle){screenWidth-170, 170, 120, 20}, "Prädation", TextFormat("%.2f", beta), &beta, 0.01f, 1.f);
        GuiSliderBar((Rectangle){screenWidth-170, 200, 120, 20}, "Reproduktion: Räuber", TextFormat("%.2f", gammma), &gammma, 0.01f, 1.f);
        GuiSliderBar((Rectangle){screenWidth-170, 230, 120, 20}, "Todesrate: Räuber", TextFormat("%.2f", delta), &delta, 0.01f, 1.f);
        GuiSliderBar((Rectangle){screenWidth-200, 270, 120, 20}, "scale", NULL, &scale, 0.1f, 15.f);
        if (GuiButton((Rectangle){screenWidth-200, 310, 120, 20}, "reset")) {
            current_tick = (Tick){40.f, 9.f};
            ticks_size = 256;
            ticks_len = 0;
            if (ticks != NULL) {
                free(ticks);
            }
            ticks = calloc(ticks_size, sizeof(Tick));
            if (ticks == NULL) {
                fprintf(stderr, "calloc: No mem.\n");
                return 1;
            }
            camX = 0;
            camY = 0;
            camera.target = (Vector2){camX, camY};
        }
        if (GuiButton((Rectangle){screenWidth-200, 340, 120, 20}, "crash")) {
            current_tick = (Tick){2.f, 2.f};
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

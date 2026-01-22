#include <raylib.h>
#include <raymath.h>

const float WINDOW_WIDTH(800);
const float WINDOW_HEIGHT(600);

Vector2 minEdge = {-600, -600};
Vector2 maxEdge = {1550, 1040};

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AlvarezCorpuzGregorio_Homework01");

    // Texture2D background = LoadTexture("<filename>");

    Rectangle view = {(WINDOW_WIDTH/2)-250, (WINDOW_HEIGHT/2)-150, 500, 300};

    SetTargetFPS(60.0f);

    Vector2 position = {400, 300};

    int cam_type = 0;
    Camera2D camera = {0};
    camera.target = position;
    camera.offset = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        
        if(IsKeyDown(KEY_W)) {
            position.y -= 200 * delta_time;
        }
        if(IsKeyDown(KEY_A)) {
            position.x -= 200 * delta_time;
        }
        if(IsKeyDown(KEY_S)) {
            position.y += 200 * delta_time;
        }
        if(IsKeyDown(KEY_D)) {
            position.x += 200 * delta_time;
        }

        position = Vector2Clamp(position, {minEdge.x + 50, minEdge.y + 50}, {maxEdge.x - 50, maxEdge.y - 50});

        camera.target = position;

        Vector2 camTarget = position;

        camTarget = Vector2Clamp(camTarget, {minEdge.x + WINDOW_WIDTH/2, minEdge.y + WINDOW_HEIGHT/2}, {maxEdge.x - WINDOW_WIDTH/2, maxEdge.y - WINDOW_HEIGHT/2});

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(MAROON);
        // DrawTexture(background, minEdge.x + 50, minEdge.y + 50, WHITE);
        DrawRectangleLinesEx(view, 3.0f, SKYBLUE);
        DrawCircle(position.x, position.y, 100.0f, BLUE);
        EndMode2D();

        EndDrawing();

    }
    // UnloadTexture(background);
    CloseWindow();
    return 0;
}


// DELETE BEFORE SUBMISSION: clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o ispy
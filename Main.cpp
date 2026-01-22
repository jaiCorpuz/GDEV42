#include <raylib.h>
#include <raymath.h>

const float WINDOW_WIDTH(800);
const float WINDOW_HEIGHT(600);

Vector2 minEdge = {-500, -500};
Vector2 maxEdge = {1548, 1032};

bool isStatic = false;

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AlvarezCorpuzGregorio_Homework01");

    Texture2D background = LoadTexture("ispy.jpeg");
    
    Vector2 position = {400, 300};

    Rectangle view = {position.x-250, position.y-150, 500, 300};

    SetTargetFPS(60.0f);


    int cam_type = 0;
    Camera2D camera = {0};
    camera.target = position;
    camera.offset = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    camera.zoom = 0.5f;

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        
        if(IsKeyPressed(KEY_ENTER) && camera.zoom == 0.5f) {
            camera.zoom = 1.0f;
            isStatic = true;
        }
        
        if(IsKeyPressed(KEY_TAB) && camera.zoom == 1.0f) {
            camera.zoom = 0.5f;
            isStatic = false;
        }

        if (!isStatic) {
            if(IsKeyDown(KEY_W)) {
                position.y -= 200 * delta_time;
                view.y -= 200 * delta_time;
            }
            if(IsKeyDown(KEY_A)) {
                position.x -= 200 * delta_time;
                view.x -= 200 * delta_time;
            }
            if(IsKeyDown(KEY_S)) {
                position.y += 200 * delta_time;
                view.y += 200 * delta_time;
            }
            if(IsKeyDown(KEY_D)) {
                position.x += 200 * delta_time;
                view.x += 200 * delta_time;
            }
        }

        position = Vector2Clamp(position, {minEdge.x + 10, minEdge.y + 10}, {maxEdge.x - 10, maxEdge.y - 10});
        view.x = Clamp(view.x, minEdge.x, maxEdge.x - view.width);
        view.y = Clamp(view.y, minEdge.y, maxEdge.y - view.height);

        camera.target = position;

        Vector2 camTarget = position;

        camTarget = Vector2Clamp(camTarget, {minEdge.x + ((WINDOW_WIDTH/2) / camera.zoom), minEdge.y + ((WINDOW_HEIGHT/2) / camera.zoom)}, {maxEdge.x - ((WINDOW_WIDTH/2) / camera.zoom), maxEdge.y - ((WINDOW_HEIGHT/2) / camera.zoom)});

        camera.target = camTarget;

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(MAROON);
        DrawTexture(background, minEdge.x, minEdge.y, WHITE);
        DrawRectangleLinesEx(view, 10.0f, SKYBLUE);
        // DrawCircle(position.x, position.y, 10.0f, BLUE);
        EndMode2D();

        EndDrawing();

    }
    UnloadTexture(background);
    CloseWindow();
    return 0;
}


// DELETE BEFORE SUBMISSION: clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o ispy
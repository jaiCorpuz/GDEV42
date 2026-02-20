#include "raylib.h"
#include "Player.cpp"
#include "Enemy.cpp"
#include <vector>

static std::ios_base::Init iostream_initializer;

const float WINDOW_WIDTH(1280);
const float WINDOW_HEIGHT(720);

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 720, "AlvarezCorpuzGregorio_Homework02");
    SetTargetFPS(60);
    
    int cam_type = 0;

    Rectangle camera_window;
    Camera2D camera_view = {0};
    Vector2 initial_offset = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    camera_view.offset = initial_offset;
    camera_view.zoom = 1.0f;

    Player player({400, 200}, 20, 200);
    player.camera = &camera_view;
    camera_window = {
        WINDOW_WIDTH / 4,
        WINDOW_HEIGHT / 4,
        WINDOW_WIDTH / 2,
        WINDOW_HEIGHT / 2
    };
    
    // std::vector<Enemy> enemies;
    // enemies.push_back(Enemy({700, 400}, 50, 100));
    Enemy enemy({600, 400}, 50, 100);
    
    // for (int i = 0; i < enemies.size(); i++) {
    //     enemies.at(i).playerRef = &player;
    // }
    enemy.playerRef = &player;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        camera_view.target = player.position;
        
        player.Update(deltaTime);
        // for (int i = 0; i < enemies.size(); i++) {
        //     enemies.at(i).Update(deltaTime);
        // }
        if (enemy.alive) {
            enemy.Update(deltaTime);
        }
        
        BeginDrawing();
        BeginMode2D(camera_view);
        ClearBackground(RAYWHITE);

        player.Draw();
        // for (int i = 0; i < enemies.size(); i++) {
        //     enemies.at(i).Draw();
        // }
        enemy.Draw();
        
        EndMode2D();

        DrawText(TextFormat("%.1f", player.hp), 20, 20, 35, BLACK);
        
        if (player.hp <= 0) {
            DrawRectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, RAYWHITE);
            DrawText("YOU LOSE", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 100, BLACK);
        } else if (!enemy.alive) {
            DrawRectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, RAYWHITE);
            DrawText("YOU WIN", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 100, BLACK);
        }

        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
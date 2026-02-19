#include "raylib.h"
#include "Player.cpp"
#include "Enemy.cpp"

int main() {
    InitWindow(1280, 720, "AlvarezCorpuzGregorio_Homework02");
    SetTargetFPS(60);

    Player player({400, 300}, 20, 200);

    Enemy enemy({600, 400}, 50, 100);
    enemy.aggroRadius = 300;
    enemy.detectionRadius = 200;
    enemy.attackRadius = 100;

    enemy.playerRef = &player;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        player.Update(deltaTime);
        enemy.Update(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        player.Draw();
        enemy.Draw();
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.detectionRadius, LIGHTGRAY);
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.aggroRadius, ORANGE);
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.attackRadius, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}


//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
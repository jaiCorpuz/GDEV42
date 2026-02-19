#include "raylib.h"
#include "Player.cpp"
#include "Enemy.cpp"

const float WINDOW_WIDTH(1280);
const float WINDOW_HEIGHT(720);

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 720, "AlvarezCorpuzGregorio_Homework02");
    SetTargetFPS(60);

    Player player({400, 200}, 20, 200);

    Enemy enemy({600, 400}, 50, 100);
    enemy.aggroRadius = 300;
    enemy.detectionRadius = 200;
    enemy.attackRadius = 100;

    enemy.playerRef = &player;

    Rectangle enemyRect = {
        enemy.position.x,
        enemy.position.y,
        enemy.size,
        enemy.size
    };

    bool collided = CheckCollisionCircleRec(
        player.position,
        player.radius,
        enemyRect
    );

    if (collided) {
        player.TakeDamage(1.0f);
    }

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        player.Update(deltaTime);
        enemy.Update(deltaTime);

        Rectangle enemyRect = {
            enemy.position.x,
            enemy.position.y,
            enemy.size,
            enemy.size
        };

        if (CheckCollisionCircleRec(player.position, player.radius, enemyRect)) {
            player.TakeDamage(1.0f);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        player.Draw();
        enemy.Draw();
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.detectionRadius, LIGHTGRAY);
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.aggroRadius, ORANGE);
        DrawCircleLines(enemy.position.x + enemy.size/2, enemy.position.y + enemy.size/2, enemy.attackRadius, RED);

        DrawText(TextFormat("%.0f", player.hp), 20, 20, 35, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}


//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
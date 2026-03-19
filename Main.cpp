#include "raylib.h"
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 768;
const int SCREEN_HEIGHT = 640;

int main()
{
    static std::ios_base::Init iostream_initializer;
    srand(time(NULL));
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AlvarezCorpuzGregorio_Homework04");
    
    // GenerateDungeon();
    
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R)) {
            // GenerateDungeon();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o dungeon
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
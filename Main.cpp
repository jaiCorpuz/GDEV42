#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

static ios_base::Init iostream_initializer;

const float WINDOW_WIDTH(1280);
const float WINDOW_HEIGHT(720);
const float playerSize = 20.0f;
const float tileScale = 2.0f;

Vector2 minEdge = {-500.0f, -500.0f};
Vector2 maxEdge = {1780.0f, 1220.0f};

struct TileType {
    Rectangle tile;
    bool isCollider;
};


int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AlvarezCorpuzGregorio_Homework03");

    SetTargetFPS(60.0f);
    
    Vector2 position = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};

    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = position;
    camera_view.offset = {WINDOW_WIDTH /2 , WINDOW_HEIGHT / 2};
    camera_view.zoom = 1.0f;

    string imageName;
    vector<TileType> tileTypes;
    vector<Rectangle> tileMap;
    vector<vector<int>> grid;
    vector<Vector2> enemies;
    int gridColumns = 0, gridRows = 0;

    ifstream file("settings.txt");
    string line;

    while (getline(file, line)) {
        istringstream stream(line);
        string key;
        stream >> key;

        if (key == "IMAGE_NAME") {
            stream >> imageName;
        } 
        else if (key == "TILE_COUNT") {
            int count;
            stream >> count;
            for (int i = 0; i < count; i++) {
                float x, y, w, h;
                int collidable;
                getline(file, line);
                istringstream tileStream(line);
                tileStream >> x >> y >> w >> h >> collidable;
                tileTypes.push_back({{x, y, w, h}, (bool) collidable});
                tileMap.push_back({x, y, w, h}); // ADD THIS LINE
            }
        }
        else if (key == "GRID") {
            stream >> gridColumns >> gridRows;
            grid.resize(gridRows, vector<int>(gridColumns));
            for (int y = 0; y < gridRows; y++) {
                getline(file, line);
                istringstream gridStream(line);
                for (int x = 0; x < gridColumns; x++) {
                    gridStream >> grid[y][x];
                }
            }
        }
    }
    file.close();

    Texture2D tileSet = LoadTexture(imageName.c_str());

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

        position = Vector2Clamp(position, {minEdge.x +30, minEdge.y +30}, {maxEdge.x - 30, maxEdge.y - 30});

        camera_view.target = position;

        camera_view.offset = {WINDOW_WIDTH /2 , WINDOW_HEIGHT / 2};

        float camera_view_Left = camera_view.target.x - camera_view.offset.x;
        float camera_view_Right = camera_view_Left + WINDOW_WIDTH;
        float camera_view_Top = camera_view.target.y - camera_view.offset.y;
        float camera_view_Bottom = camera_view_Top + WINDOW_HEIGHT;

        // Clamp X
        if (camera_view_Left <= minEdge.x) {
            camera_view.offset.x = camera_view.target.x - minEdge.x;
        }
        else if (camera_view_Right >= maxEdge.x) {
            camera_view.offset.x = WINDOW_WIDTH - (maxEdge.x - camera_view.target.x);
        }

        // Clamp Y
        if (camera_view_Top <= minEdge.y) {
            camera_view.offset.y = camera_view.target.y - minEdge.y;
        }
        else if (camera_view_Bottom >= maxEdge.y) {
            camera_view.offset.y = WINDOW_HEIGHT - (maxEdge.y - camera_view.target.y);
        }

        BeginDrawing();
        BeginMode2D(camera_view);
        ClearBackground(RAYWHITE);

        for (int y = 0; y < gridRows; y++) {
            for (int x = 0; x < gridColumns; x++) {
                int tileID = grid[y][x];

                if (tileID >= 0 && tileID < tileMap.size()) {
                    Rectangle tile = tileMap[tileID];

                    Rectangle position = {
                        (float) x * (tile.width*tileScale),
                        (float) y * (tile.height*tileScale),
                        tile.width * tileScale,
                        tile.height * tileScale
                    };

                    DrawTexturePro(tileSet, tile, position, {0, 0}, 0.0f, WHITE);
                }
            }
        }
        // DrawRectangle(120, 100, 40, 80, BLUE);
        DrawCircle(position.x, position.y, 30.0f, DARKBLUE);
        EndMode2D();
        // Draw UI after EndMode2D
        EndDrawing();
    }
    UnloadTexture(tileSet);
    CloseWindow();
    return 0;
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o map
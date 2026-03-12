#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "Player.cpp"
#include "Enemy.cpp"

using namespace std;

static ios_base::Init iostream_initializer;

const float WINDOW_WIDTH(1280);
const float WINDOW_HEIGHT(720);
const float playerSize = 20.0f;

Vector2 minEdge = {0.0f, 0.0f};
Vector2 maxEdge = {2150.0f, 1440.0f};

vector<TileType> tileTypes;

bool CheckTileCollision(
    Vector2 testPosition,
    float radius,
    const vector<vector<int>>& grid,
    const vector<TileType>& tileTypes,
    float tileScale,
    int gridRows,
    int gridColumns
) {
    for (int y = 0; y < gridRows; y++) {
        for (int x = 0; x < gridColumns; x++) {

            int id = grid[y][x];

            if (tileTypes[id].isCollidable) {

                Rectangle tileRect = {
                    (float)x * (tileTypes[id].source.width * tileScale),
                    (float)y * (tileTypes[id].source.height * tileScale),
                    tileTypes[id].source.width * tileScale,
                    tileTypes[id].source.height * tileScale
                };

                if (CheckCollisionCircleRec(testPosition, radius, tileRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//Collision handler for both player and enemy
void HandleCollisions(Player& player, vector<Enemy>& enemies) {
    for (auto& e : enemies) {
        if (!e.alive) continue;

        if (CheckCollisionCircleRec(player.position, player.radius, {e.position.x, e.position.y, e.size, e.size})) {
            player.TakeDamage(1.0f);
        }

        if (dynamic_cast<PlayerAttacking*>(player.GetCurrentState())) {
            float distance = Vector2Distance(player.position, e.position);
            if (distance < player.radius + e.size / 2.0f) {
                e.TakeDamage();
            }
        }
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AlvarezCorpuzGregorio_Homework03");

    SetTargetFPS(60.0f);
    
    string imageName;
    vector<TileType> tileTypes;
    vector<Rectangle> tileMap;
    vector<vector<int>> grid;
    vector<Enemy> enemies; // for some reason current_state->Update does not work here
    int enemy_count;
    Enemy e1 = Enemy({-100, -100}, 25, 50);
    Enemy e2 = Enemy({-100, -100}, 25, 50);
    Enemy e3 = Enemy({-100, -100}, 25, 50);
    Vector2 position;
    float tileScale;
    int gridColumns = 0, gridRows = 0;
    Vector2 playerPosition;
    
    ifstream settings("settings.txt");
    string line;
    
    while (getline(settings, line)) {
        istringstream stream(line);
        string key;
        stream >> key;
        
        if (key == "IMAGE_NAME") {
            stream >> imageName;
        } 
        else if (key == "SCALE") {
            stream >> tileScale;
        }
        else if (key == "PLAYER_START") {
            stream >> position.x >> position.y;
            playerPosition = position;
        }
        else if (key == "TILE_COUNT") {
            int count;
            stream >> count;
            for (int i = 0; i < count; i++) {
                float x, y, w, h;
                int collidable;
                getline(settings, line);
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
                getline(settings, line);
                istringstream gridStream(line);
                for (int x = 0; x < gridColumns; x++) {
                    gridStream >> grid[y][x];
                }
            }
        }
    }
    settings.close();
    
    ifstream save("save_file.txt");
    while (getline(save, line)) {
        istringstream stream(line);
        string key;
        stream >> key;

        if (key == "PLAYER_START") {
            stream >> position.x >> position.y;
            playerPosition = position;
        } else if (key == "ENEMY_COUNT") {
            stream >> enemy_count;
            // for (int i = 0; i < enemy_count; i++) {
            //     float x, y;
            //     getline(settings, line);
            //     istringstream enemyStream(line);
            //     enemyStream >> x >> y;
            //     enemies.push_back(Enemy({x,y}, 25, 10));
            // }
            float ex = 0, ey = 0;
            getline(save, line);
            istringstream enemyStream1(line);
            enemyStream1 >> ex >> ey;
            std::cout << " " << ex << " " << ey << std::endl;
            e1.position.x = ex;
            e1.position.y = ey;

            getline(save, line);
            istringstream enemyStream2(line);
            enemyStream2 >> ex >> ey;
            std::cout << " " << ex << " " << ey << std::endl;
            e2.position.x = ex;
            e2.position.y = ey;

            getline(save, line);
            istringstream enemyStream3(line);
            enemyStream3 >> ex >> ey;
            std::cout << " " << ex << " " << ey << std::endl;
            e3.position.x = ex;
            e3.position.y = ey;
        }

    }
    save.close();

    Texture2D tileSet = LoadTexture(imageName.c_str());

    // camera settings
    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = playerPosition;
    camera_view.offset = {WINDOW_WIDTH /2 , WINDOW_HEIGHT / 2};
    camera_view.zoom = 1.0f;

    // player settings
    Player player(playerPosition, playerSize, 200.0f); //replace speed
    player.camera = &camera_view;
    player.grid = grid;
    player.tileTypes = tileTypes;
    player.tileScale = tileScale;
    player.gridRows = gridRows;
    player.gridColumns = gridColumns;
    
    e1.grid = grid;
    e1.tileTypes = tileTypes;
    e1.tileScale = tileScale;
    e1.gridRows = gridRows;
    e1.gridColumns = gridColumns;

    e2.grid = grid;
    e2.tileTypes = tileTypes;
    e2.tileScale = tileScale;
    e2.gridRows = gridRows;
    e2.gridColumns = gridColumns;

    e3.grid = grid;
    e3.tileTypes = tileTypes;
    e3.tileScale = tileScale;
    e3.gridRows = gridRows;
    e3.gridColumns = gridColumns;

    // for (int i = 0; i < enemy_count; i++) {
    //     enemies.at(i).playerRef = &player;
    // }
    e1.playerRef = &player;
    e2.playerRef = &player;
    e3.playerRef = &player;
    
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        float speed = 200 * delta_time;
        
        player.Update(delta_time);
        for (int i = 0; i < enemy_count; i++)
        {
            // enemies.at(i).Update(delta_time);
        }
        e1.Update(delta_time);
        e2.Update(delta_time);
        e3.Update(delta_time);

        player.position = Vector2Clamp(player.position, {minEdge.x +30, minEdge.y +30}, {maxEdge.x - 30, maxEdge.y - 30});

        camera_view.target = player.position;
        // cout << "Player position: (" << player.position.x << ", " << player.position.y << ")\n";

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
                    TileType tile = tileTypes[tileID];
                    Rectangle position = {
                        (float) x * (tile.source.width*tileScale),
                        (float) y * (tile.source.height*tileScale),
                        tile.source.width * tileScale,
                        tile.source.height * tileScale
                    };
                    DrawTexturePro(tileSet, tile.source, position, {0, 0}, 0.0f, WHITE);
                }
            }
        }

        player.Draw();
        // for (int i = 0; i < enemy_count; i++)
        // {
        //     enemies.at(0).Draw();
        // }
        e1.Draw();
        e2.Draw();
        e3.Draw();
        EndMode2D();
        // Draw UI after EndMode2D
        EndDrawing();
    }
    UnloadTexture(tileSet);
    CloseWindow();
    if (WindowShouldClose()) {
        ofstream save_file("save_file.txt");
        save_file << "PLAYER_START " << player.position.x << " " << player.position.y << " " << std::endl;
        save_file << "ENEMY_COUNT " << enemy_count << std::endl;
        // for (int i = 0; i < enemy_count; i++)
        // {
        //     save_file << enemies.at(i).position.x << " " << enemies.at(i).position.y << std::endl;
        // }
        save_file << e1.position.x << " " << e1.position.y << std::endl;
        save_file << e2.position.x << " " << e2.position.y << std::endl;
        save_file << e3.position.x << " " << e3.position.y << std::endl;
        
        save_file.close();
    }
    return 0;
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o level
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
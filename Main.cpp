#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <bits/stdc++.h>
#include <vector>
#include <algorithm>
#include <string>


#include "Player.cpp"
#include "Room.cpp"
#include "Tile.cpp"

using namespace std;

const int SCREEN_TILE_WIDTH = 12;
const int SCREEN_TILE_HEIGHT = 10;
// const int SCREEN_WIDTH = 768;
// const int SCREEN_HEIGHT = 640;

int main()
{
    static std::ios_base::Init iostream_initializer;
    srand(time(NULL));
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    
    // Get tilemap information
    
    string tilemap_filename;
    int tile_size;
    float tile_scale;
    vector<Tile> tile_types;
    
    ifstream settings("tilemap.txt");
    string line;
    
    while (getline(settings, line)) {
        istringstream stream(line);
        string key;
        stream >> key;
        
        if (key == "IMAGE_NAME") {
            stream >> tilemap_filename;
        } 
        else if (key == "TILE_SIZE") {
            stream >> tile_size;
        }
        else if (key == "TILE_SCALE") {
            stream >> tile_scale;
        }
        else if (key == "TILE_COUNT") {
            int count;
            stream >> count;
            for (int i = 0; i < count; i++) {
                float x, y;
                int collidable;
                getline(settings, line);
                istringstream tile_stream(line);
                tile_stream >> x >> y >> collidable;
                Rectangle tile_source = {
                    x*tile_size,
                    y*tile_size,
                    (float) tile_size,
                    (float) tile_size,
                };
                tile_types.push_back(Tile(tile_source, (bool) collidable));
            }
        }
    }
    settings.close();

    int screen_width = SCREEN_TILE_WIDTH * tile_size * tile_scale;
    int screen_height = SCREEN_TILE_HEIGHT * tile_size * tile_scale;
    InitWindow(screen_width, screen_height, "AlvarezCorpuzGregorio_Homework04");
    
    Texture2D tilemap = LoadTexture(tilemap_filename.c_str());
    
    // GenerateDungeon();

    Player player({(float)screen_width/2.0f,(float)screen_height/2.0f}, 25, 200.0f);

    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = player.position;
    camera_view.offset = {(float) screen_width /2 , (float)screen_height / 2};
    camera_view.zoom = 1.0f;
    
    vector<Room*> created_rooms = GenerateDungeon();
    
    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();

        int roomX = floor(player.position.x/screen_width);
        int roomY = floor(player.position.y/screen_height);

        Vector2 desiredTarget = {
            (roomX * screen_width) + (screen_width / 2.0f),
            (roomY * screen_height) + (screen_height / 2.0f)
        };
        
        camera_view.target = Vector2Lerp(camera_view.target, desiredTarget, 0.009f);

        // camera_view.target = player.position;
        player.Update(delta_time);
        
        if (IsKeyPressed(KEY_R)) {
            created_rooms = GenerateDungeon();
        }

        BeginDrawing();
        BeginMode2D(camera_view);
        ClearBackground(BLACK);
        
        // DrawTexture(tilemap, 0,0, WHITE);
        for (Room* r: created_rooms) {
            for (int i = 0; i < SCREEN_TILE_HEIGHT; i++) {
                for (int j = 0; j < SCREEN_TILE_WIDTH; j++) {
                    int tile_type = 12;
                    // draw edges
                    if (i == 0 && j == 0) {
                        tile_type = 0;
                    } else if (i == 0 && j == SCREEN_TILE_WIDTH-1) {
                        tile_type = 2;
                    } else if (i == SCREEN_TILE_HEIGHT-1 && j == 0) {
                        tile_type = 5;
                    } else if (i == SCREEN_TILE_HEIGHT-1 && j == SCREEN_TILE_WIDTH-1) {
                        tile_type = 6;
                    } else if (j == 0) {
                        tile_type = 3;
                    } else if (i == 0 || i == SCREEN_TILE_HEIGHT-1) {
                        tile_type = 1;
                    } else if (j == SCREEN_TILE_WIDTH-1) {
                        tile_type = 4;
                    }
                    // draw conenctions
                    int hallway_width = 2;
                    int left_corner = (SCREEN_TILE_WIDTH/2)-hallway_width;
                    int right_corner = (SCREEN_TILE_WIDTH/2)+hallway_width-1;
                    int up_corner = (SCREEN_TILE_HEIGHT/2)-hallway_width;
                    int down_corner = (SCREEN_TILE_HEIGHT/2)+hallway_width-1;
                    if (r->neighbors.at(0) != nullptr && r->neighbors.at(0)->type != EMPTY) {
                        if (i==0) {
                            if (j==left_corner) {
                                tile_type = 10;
                            } else if (j==right_corner) {
                                tile_type = 9; 
                            } else if (j > left_corner && j < right_corner) {
                                tile_type = 12;
                            }
                        }
                    }
                    if (r->neighbors.at(3) != nullptr && r->neighbors.at(3)->type != EMPTY) {
                        if (i==SCREEN_TILE_HEIGHT-1) {
                            if (j==left_corner) {
                                tile_type = 8;
                            } else if (j==right_corner) {
                                tile_type = 7; 
                            } else if (j > left_corner && j < right_corner) {
                                tile_type = 12;
                            }
                        }
                    }
                    if (r->neighbors.at(1) != nullptr && r->neighbors.at(1)->type != EMPTY) {
                        if (j==0) {
                            if (i==up_corner) {
                                tile_type = 10;
                            } else if (i==down_corner) {
                                tile_type = 8; 
                            } else if (i > up_corner && i < down_corner) {
                                tile_type = 12;
                            }
                        }
                    }
                    if (r->neighbors.at(2) != nullptr && r->neighbors.at(2)->type != EMPTY) {
                        if (j==SCREEN_TILE_WIDTH-1) {
                            if (i==up_corner) {
                                tile_type = 9;
                            } else if (i==down_corner) {
                                tile_type = 7; 
                            } else if (i > up_corner && i < down_corner) {
                                tile_type = 12;
                            }
                        }
                    }

                    if (tile_types[tile_type].isCollidable) {
                        r->collidable_tiles.push_back((Vector2){static_cast<float>(j),static_cast<float>(i)});
                    }

                    DrawTexturePro(
                        tilemap,
                        tile_types[tile_type].source,
                        {
                            ((float)j*tile_size*tile_scale) + (screen_width * r->position.x),
                            ((float)i*tile_size*tile_scale) + (screen_height * r->position.y),
                            tile_size*tile_scale,
                            tile_size*tile_scale},
                        {0,0},
                        0,
                        WHITE
                    );
                }
            }
        }
        
        
        
        player.Draw();
        EndMode2D();
        EndDrawing();
    }

    UnloadTexture(tilemap);

    CloseWindow();

    return 0;
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o dungeon
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
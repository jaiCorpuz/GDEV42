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
#include "Enemy.cpp"
// #include "Room.cpp"
// #include "Tile.cpp"

using namespace std;

//The Gamescreen for winning and losing and playing
enum GameScreen { 
    PLAYING, 
    GAMEOVER, 
    WIN };

const int SCREEN_TILE_WIDTH = 12;
const int SCREEN_TILE_HEIGHT = 10;

int Tile::size;
int Tile::scale;

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
            Tile::size = tile_size;
        }
        else if (key == "TILE_SCALE") {
            stream >> tile_scale;
            Tile::scale = tile_scale;
        }
        else if (key == "TILE_COUNT") {
            int count;
            stream >> count;
            for (int i = 0; i < count; i++) {
                float x, y;
                int collidable, collectable, unlockable;
                getline(settings, line);
                istringstream tile_stream(line);
                tile_stream >> x >> y >> collidable >> collectable >> unlockable;
                Rectangle tile_source = {
                    x*tile_size,
                    y*tile_size,
                    (float) tile_size,
                    (float) tile_size,
                };
                tile_types.push_back(Tile(tile_source, (bool) collidable, (bool) collectable, (bool) unlockable));
            }
        }
    }
    settings.close();

    int screen_width = SCREEN_TILE_WIDTH * tile_size * tile_scale;
    int screen_height = SCREEN_TILE_HEIGHT * tile_size * tile_scale;
    InitWindow(screen_width, screen_height, "AlvarezCorpuzGregorio_Homework04");
    
    Texture2D tilemap = LoadTexture(tilemap_filename.c_str());

    Player player({(float)screen_width/2.0f,(float)screen_height/2.0f}, screen_height/25.6f, (float)screen_height/8*3);

    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = player.position;
    camera_view.offset = {(float) screen_width /2 , (float)screen_height / 2};
    camera_view.zoom = 1.0f;
    
    vector<Room*> created_rooms = GenerateDungeon();
    // RoomCollisions(created_rooms, tile_types);

    // Enemy bossEnemy({0, 0}, 30.0f, 150.0f);
    // bossEnemy.playerRef = &player;
    
    // //places the boss in the boss room
    // for (Room* r : created_rooms) {
    //     if (r->type == BOSS) {
    //         bossEnemy.current_room = r;
    //         bossEnemy.position.x = (r->position.x * screen_width) + (screen_width / 2.0f);
    //         bossEnemy.position.y = (r->position.y * screen_height) + (screen_height / 2.0f);
    //         bossEnemy.hp = 2.0f;
    //         bossEnemy.alive = true;
    //         bossEnemy.SetState(&bossEnemy.wandering);
    //         break;
    //     }
    // }

    // bossEnemy replaced with a vector of enemy pointers
    vector<Enemy*> dungeon_enemies;
    
    // Spawn enemies in each room
    for (Room* r : created_rooms) {
        if (r->type != START) {
            int num_enemies = GetRandomValue(1, 3); // 0 to 3 enemies per room
            
            for (int i = 0; i < num_enemies; i++) {

                int enemyType = GetRandomValue(0, 2);
                Enemy* e = nullptr;

              
                float ex = (r->position.x * screen_width) + GetRandomValue(100, screen_width - 100);
                float ey = (r->position.y * screen_height) + GetRandomValue(100, screen_height - 100);

                if (enemyType == 0) {
                    e = new Shadow({0,0}, 30.0f, 120.0f);
                } else if (enemyType == 1) {
                    e = new Spirit({0,0}, 30.0f, 100.0f);
                } else {
                    e = new Poltergeist({0,0}, 30.0f, 150.0f);
                }
                
                //Enemy* e = new Enemy({0, 0}, 30.0f, 100.0f); 
                e->playerRef = &player;
                e->current_room = r;
                e->alive = true;
                e->position = {ex, ey};
                e->SetState(&e->wandering);
                dungeon_enemies.push_back(e);

                

            }
        }
    }
    
    GameScreen currentScreen = PLAYING;
    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();

        //while the game is in playing mode (not lose or win) continue
        if(currentScreen == PLAYING){
            int roomX = floor(player.position.x/screen_width);
            int roomY = floor(player.position.y/screen_height);

            Vector2 desiredTarget = {
                (roomX * screen_width) + (screen_width / 2.0f),
                (roomY * screen_height) + (screen_height / 2.0f)
            };
            
            camera_view.target = Vector2Lerp(camera_view.target, desiredTarget, 0.05f);

            player.Update(delta_time);

            for (Room* r: created_rooms) {
                if (
                    r->position.x == std::floor(player.position.x / screen_width) &&
                    r->position.y == std::floor(player.position.y / screen_height)
                ) {
                    player.current_room = r;
                    break;
                }
            }

            bool allEnemiesDead = true; 

            for (Enemy* e : dungeon_enemies) {
                if (e->alive) {
                    allEnemiesDead = false; 
                    
                    int eRoomX = floor(e->position.x / screen_width);
                    int eRoomY = floor(e->position.y / screen_height);
                    e->Update(delta_time);
                }
            } 
            
            
            if (player.hp <= 0) {
                currentScreen = GAMEOVER;
            } else if (allEnemiesDead && !dungeon_enemies.empty()) {
                currentScreen = WIN; //temptemptemp
            }
            
            // if (bossEnemy.alive) {

            //     int bossRoomX = floor(bossEnemy.position.x / screen_width);
            //     int bossRoomY = floor(bossEnemy.position.y / screen_height);

            //     //if the player enters the boss room, that is when the boss moves
            //     if (roomX == bossRoomX && roomY == bossRoomY) {
            //         bossEnemy.Update(delta_time);
            //     }
            // }     
            
            //gameover if the player loses health, win if the player kills boss.
            // if (player.hp <= 0) {
            //     currentScreen = GAMEOVER;
            // }
            // } else if (!bossEnemy.alive) {
            //     currentScreen = WIN;
            // }

        }

        //reset 
        if (IsKeyPressed(KEY_R)) {
            for (Room* r: created_rooms) {
                delete r;
            }
            created_rooms.clear();

            created_rooms = GenerateDungeon();
            // RoomCollisions(created_rooms, tile_types);

            for (Room* r : created_rooms) {
                if (r->type == START) {
                    player.position.x = (r->position.x * screen_width) + (screen_width/2.0f);
                    player.position.y = (r->position.y * screen_height) + (screen_height/2.0f);
                    break;
                }
            }

            player.key_collected = false;

            // Memory cleanup for enemies
            for (Enemy* e : dungeon_enemies) {
                delete e;
            }
            dungeon_enemies.clear();

            // Re-spawn enemies for the new dungeon
            for (Room* r : created_rooms) {
                if (r->type != START) {
                    int num_enemies = GetRandomValue(0, 3); // 1 to 3 enemies per room
                    
                    for (int i = 0; i < num_enemies; i++) {

                        int enemyType = GetRandomValue(0, 2);
                        Enemy* e = nullptr;

                        // 2. Create the specific subclass
                        float ex = (r->position.x * screen_width) + GetRandomValue(100, screen_width - 100);
                        float ey = (r->position.y * screen_height) + GetRandomValue(100, screen_height - 100);

                        if (enemyType == 0) {
                            e = new Shadow({0,0}, 30.0f, 120.0f);
                        } else if (enemyType == 1) {
                            e = new Spirit({0,0}, 30.0f, 100.0f);
                        } else {
                            e = new Poltergeist({0,0}, 30.0f, 150.0f);
                        }

                        //Enemy* e = new Enemy({0, 0}, 30.0f, 100.0f);
                        e->playerRef = &player;
                        e->current_room = r;
                        e->alive = true;
                        e->position = {ex, ey};
                        e->SetState(&e->wandering);
                        dungeon_enemies.push_back(e);

                    
                    }
                }
            }

            player.hp = 5.0f;
            currentScreen = PLAYING;
        }

        BeginDrawing();
        ClearBackground((Color) {0xda, 0x7a, 0x34, 0xff});
        if(currentScreen == PLAYING){
            BeginMode2D(camera_view);
        
            for (Room* r: created_rooms) {
                if (
                    player.current_room == r ||
                    (player.current_room->neighbors.at(NORTH) != nullptr && player.current_room->neighbors.at(NORTH) == r) ||
                    (player.current_room->neighbors.at(WEST) != nullptr && player.current_room->neighbors.at(WEST) == r) ||
                    (player.current_room->neighbors.at(EAST) != nullptr && player.current_room->neighbors.at(EAST) == r) ||
                    (player.current_room->neighbors.at(SOUTH) != nullptr && player.current_room->neighbors.at(SOUTH) == r)
                ) {
                    // std::cout << TextFormat("drawing room %.0f %.0f", r->position.x, r->position.y) << std::endl;
                    for (int i = 0; i < SCREEN_TILE_HEIGHT; i++) {
                        for (int j = 0; j < SCREEN_TILE_WIDTH; j++) {
                            // std::cout << "t";
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
                                        if (r->neighbors.at(0) != nullptr && r->neighbors.at(0)->is_locked) {
                                            tile_type = 16;
                                        } else {
                                        tile_type = 12;
                                        }
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
                                        if (r->neighbors.at(3) != nullptr && r->neighbors.at(3)->is_locked) {
                                            tile_type = 16;
                                        } else {
                                            tile_type = 12;
                                        }
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
                                        if (r->neighbors.at(1) != nullptr && r->neighbors.at(1)->is_locked) {
                                            tile_type = 16;
                                        } else {
                                            tile_type = 12;
                                        }
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
                                        if (r->neighbors.at(2) != nullptr && r->neighbors.at(2)->is_locked) {
                                            tile_type = 16;
                                        } else {
                                            tile_type = 12;
                                        }
                                    }
                                }
                            }
                            // draw key tile
                            if (r->type == KEY && !r->key_collected) {
                                if (i==SCREEN_TILE_HEIGHT/2 && j==SCREEN_TILE_WIDTH/2) {
                                    tile_type = 17;
                                }
                            }
                            if (r->type == BOSS) {
                                if (i==SCREEN_TILE_HEIGHT/2 && j==SCREEN_TILE_WIDTH/2) {
                                    tile_type = 18;
                                }
                            }

                            // add interactions to tiles
                            // this actually allows the same tile to be in collidable_tiles multiple times
                            // at least it stops after a while
                            if (tile_types[tile_type].isCollidable && r->collidable_tiles.size() < SCREEN_TILE_HEIGHT*SCREEN_TILE_WIDTH) {
                                r->collidable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }
                            if (tile_types[tile_type].isCollectable && r->collectable_tiles.size() < 1) {
                                r->collectable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }
                            if (tile_types[tile_type].isUnlockable && r->unlockable_tiles.size() < 8) {
                                r->unlockable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }

                            // draw tile
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
            }
            // std::cout << std::endl;
            
            player.Draw();
            if (player.key_collected) {
                DrawTexturePro(
                tilemap,
                {0, (float)8*tile_size, (float)tile_size, (float)tile_size},
                {player.position.x-player.radius, player.position.y-player.radius, tile_size*tile_scale, tile_size*tile_scale},
                {0,0},
                0, WHITE);
            }
            
            for (Enemy* e : dungeon_enemies) {
                // Only draw enemies that are alive and in the current room
                if (e->alive) {
                    e->Draw();
                }
            }          
            
            DrawText(TextFormat("HP: %d", (int)player.hp), (int)player.position.x - 25, (int)player.position.y - 40, 20, GREEN);
            
            EndMode2D();

            // DrawText(TextFormat("fps %.2f", 1/delta_time), 10, 10, 50, WHITE);

            if (player.obscureTimer > 0) {
                    // Draw a solid black rectangle over the entire window
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK); 
                
                }

            // minimap
            if (IsKeyDown(KEY_M)) {
                DrawRectangle(0,0,screen_width,screen_height, ColorAlpha(BLACK, 0.5f));
                Vector2 center = {(float)screen_width/2, (float)screen_height/2};
                float minimap_scale = 0.1;
                Vector2 room_dimensions = {(float)screen_width*minimap_scale, (float)screen_height*minimap_scale};
                for (Room* r: created_rooms) {
                    DrawRectangle(
                        (center.x)-(room_dimensions.x/2)+(room_dimensions.x*r->position.x),
                        (center.y)-(room_dimensions.y/2)+(room_dimensions.y*r->position.y),
                        room_dimensions.x,
                        room_dimensions.y,
                        ColorAlpha(BLACK, 0.5f)
                    );
                    DrawRectangleLines(
                        (center.x)-(room_dimensions.x/2)+(room_dimensions.x*r->position.x),
                        (center.y)-(room_dimensions.y/2)+(room_dimensions.y*r->position.y),
                        room_dimensions.x,
                        room_dimensions.y,
                        WHITE
                    );
                    if (r->type == BOSS) {
                        DrawRectangle(
                            (center.x)-(room_dimensions.x/2)+(room_dimensions.x*r->position.x)+5,
                            (center.y)-(room_dimensions.y/2)+(room_dimensions.y*r->position.y)+5,
                            room_dimensions.x-10,
                            room_dimensions.y-10,
                            RED
                        );
                    } 
                    if (r->type == START) {
                        DrawRectangle(
                            (center.x)-(room_dimensions.x/2)+(room_dimensions.x*r->position.x)+5,
                            (center.y)-(room_dimensions.y/2)+(room_dimensions.y*r->position.y)+5,
                            room_dimensions.x-10,
                            room_dimensions.y-10,
                            GREEN
                        );
                    } 
                    if (r->type == KEY && !r->key_collected) {
                        DrawCircle(
                            (center.x)+(room_dimensions.x*r->position.x),
                            (center.y)+(room_dimensions.y*r->position.y),
                            screen_height/32,
                            BLUE
                        );
                    } 
                    if (r->is_locked) {
                        for (int i = 0; i < 3; i++) {
                            DrawRectangleLines(
                                (i*2)+(center.x)-(room_dimensions.x/2)+(room_dimensions.x*r->position.x)+5,
                                (i*2)+(center.y)-(room_dimensions.y/2)+(room_dimensions.y*r->position.y)+5,
                                room_dimensions.x-10-(i*4),
                                room_dimensions.y-10-(i*4),
                                BLUE
                            );
                        }
                    } 
                }
                
                DrawCircle(
                    center.x + std::floor(player.position.x/screen_width)*room_dimensions.x,
                    center.y + std::floor(player.position.y/screen_height)*room_dimensions.y,
                    screen_height/64,
                    WHITE
                );
            }
        } else if (currentScreen == GAMEOVER) {
            DrawText("GAMEOVER :(", screen_width/2 - MeasureText("GAMEOVER :(", 60)/2, screen_height/2 - 40, 60, RED);
            DrawText("Press 'R' to Restart", screen_width/2 - MeasureText("Press 'R' to Restart", 30)/2, screen_height/2 + 40, 30, WHITE);
           
        } else if (currentScreen == WIN) {
            DrawText("YOU WIN!", screen_width/2 - MeasureText("YOU WIN!", 60)/2, screen_height/2 - 40, 60, GOLD);
            DrawText("Press 'R' to Restart", screen_width/2 - MeasureText("Press 'R' to Restart", 30)/2, screen_height/2 + 40, 30, WHITE);
           
        }
    
        EndDrawing();
    }

    UnloadTexture(tilemap);
    for (Enemy* e : dungeon_enemies) {
            delete e;
    }
    dungeon_enemies.clear();

    CloseWindow();

    return 0;
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o dungeon
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
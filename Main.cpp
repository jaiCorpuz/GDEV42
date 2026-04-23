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

int Tile::size;
int Tile::scale;

Color level_colors[3] = {WHITE, GREEN, RED};
//A function that spawns enemies in the dungeon 
void DungeonEnemySpawner(
    std::vector<Enemy*>& enemies,
    std::vector<Room*>& rooms,
    Player& player,
    int screen_width,
    int screen_height, 
    int tile_size,
    float tile_scale
) {
    // For each room, spawn a random number of enemies (between 1 and 3) at random positions within the room, except for the starting room.
    for (Room* r : rooms) {
        if (r->type == START) continue;

        int num_enemies = GetRandomValue(1, 3);

        for (int i = 0; i < num_enemies; i++) {

            int enemyType = GetRandomValue(0, 2);
            Enemy* e = nullptr;

            //Calcuates the walls
            float wall_thickness = tile_size * tile_scale;
            float enemy_radius = 30.0f;

            //to prevent spawning inside walls
            int min_x = (int)(wall_thickness + enemy_radius);
            int max_x = (int)(screen_width - wall_thickness - enemy_radius);

            int min_y = (int)(wall_thickness + enemy_radius);
            int max_y = (int)(screen_height - wall_thickness - enemy_radius);

            //position enemy in safe area that isnt a wall
            float ex = (r->position.x * screen_width) + GetRandomValue(min_x, max_x);
            float ey = (r->position.y * screen_height) + GetRandomValue(min_y, max_y);

            if (enemyType == 0) {
                e = new Shadow({0,0}, 30.0f, 120.0f);
            } else if (enemyType == 1) {
                e = new Spirit({0,0}, 30.0f, 100.0f);
            } else {
                e = new Poltergeist({0,0}, 30.0f, 150.0f);
            }

            e->playerRef = &player;
            e->current_room = r;
            e->alive = true;
            e->position = {ex, ey};
            e->SetState(&e->wandering);

            enemies.push_back(e);
        }
    }
}



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
                string s_interactions;
                unsigned int interactions;
                getline(settings, line);
                istringstream tile_stream(line);
                tile_stream >> x >> y;
                tile_stream >> s_interactions;
                interactions = stoi(s_interactions, 0, 2);
                Rectangle tile_source = {
                    x*tile_size,
                    y*tile_size,
                    (float) tile_size,
                    (float) tile_size,
                };
                tile_types.push_back(Tile(tile_source, interactions));
            }
        }
    }
    settings.close();

    screen_width = SCREEN_TILE_WIDTH * tile_size * tile_scale;
    screen_height = SCREEN_TILE_HEIGHT * tile_size * tile_scale;
    InitWindow(screen_width, screen_height, "AlvarezCorpuzGregorio_Homework04");
    
    Texture2D tilemap = LoadTexture(tilemap_filename.c_str());

    Player player({(float)screen_width/2.0f,(float)screen_height/2.0f}, screen_height/25.6f, (float)screen_height/8*3);

    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = player.position;
    camera_view.offset = {(float) screen_width /2 , (float)screen_height / 2};
    camera_view.zoom = 1.0f;
    player.camera = &camera_view;   //Knows where cursor is relative to player position
    
    vector<Room*> created_rooms = GenerateDungeon(5);
    // RoomCollisions(created_rooms, tile_types);

    Enemy* bossEnemy = nullptr;
    for (Room* r : created_rooms) {
        if (r->type == BOSS) {

            bossEnemy = new Boss({0, 0}, 40.0f, 120.0f);

            bossEnemy->playerRef = &player;
            bossEnemy->current_room = r;
            bossEnemy->alive = true;

            bossEnemy->position = {
                (r->position.x * screen_width) + (screen_width / 2.0f),
                (r->position.y * screen_height) + (screen_height / 2.0f)
            };

            bossEnemy->SetState(&bossEnemy->wandering);

            break; // only one boss
        }
    }

    // Spawn enemies in each room
    vector<Enemy*> dungeon_enemies;
    DungeonEnemySpawner(dungeon_enemies, created_rooms, player, screen_width, screen_height, tile_size, tile_scale);

    GameScreen currentScreen = PLAYING;
    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();

        //while the game is in playing mode (not lose or win) continue
        if(currentScreen == PLAYING){
            // std::cout << "== PLAYING" << std::endl;
            int roomX = floor(player.position.x/screen_width);
            int roomY = floor(player.position.y/screen_height);
            // std::cout << "=== ROOM X Y" << std::endl;
            
            Vector2 desiredTarget = {
                (roomX * screen_width) + (screen_width / 2.0f),
                (roomY * screen_height) + (screen_height / 2.0f)
            };
            
            camera_view.target = Vector2Lerp(camera_view.target, desiredTarget, 0.05f);
            // std::cout << "=== CAMERA" << std::endl;
            
            player.Update(delta_time);
            // std::cout << "=== PLAYER UPDATE" << std::endl;
            
            for (Room* r: created_rooms) {
                if (
                    r->position.x == std::floor(player.position.x / screen_width) &&
                    r->position.y == std::floor(player.position.y / screen_height)
                ) {
                    player.current_room = r;
                    break;
                }
            }
            // std::cout << "=== ROOM UPDATE" << std::endl;
            
            bool allEnemiesDead = true; 

            //Update enemy if they are alive. 
            for (Enemy* e : dungeon_enemies) {
                if (e->alive) {
                    allEnemiesDead = false; 
                    
                    int eRoomX = floor(e->position.x / screen_width);
                    int eRoomY = floor(e->position.y / screen_height);
                    e->Update(delta_time);
                }
            } 
            // std::cout << "=== ENEMY UPDATE" << std::endl;
            
            
            if (player.hp <= 0) {
                currentScreen = GAMEOVER;
            } else if (allEnemiesDead && !dungeon_enemies.empty()) {
                currentScreen = WIN; //temptemptemp
            }
            
            if (bossEnemy->alive) {

                int bossRoomX = floor(bossEnemy->position.x / screen_width);
                int bossRoomY = floor(bossEnemy->position.y / screen_height);

                //if the player enters the boss room, that is when the boss moves
                if (roomX == bossRoomX && roomY == bossRoomY) {
                    bossEnemy->Update(delta_time);
                }
            }     
            
            //gameover if the player loses health, win if the player kills boss.
            if (player.hp <= 0) {
                currentScreen = GAMEOVER;
            } else if (!bossEnemy->alive) {
                currentScreen = WIN;
            }

            
            // std::cout << "!= PLAYING" << std::endl;

        }

        //reset 
        if (IsKeyPressed(KEY_R) || player.level_up) {
            // std::cout << "== LEVEL UP" << std::endl;
            
            if (IsKeyPressed(KEY_R)) {
                player.level = 0;
                player.win = false;
            }

            for (Room* r: created_rooms) {
                delete r;
            }
            created_rooms.clear();

            created_rooms = GenerateDungeon(player.level_rooms[player.level]);
            // RoomCollisions(created_rooms, tile_types);

            // for (Room* r : created_rooms) {
            //     if (r->type == START) {
            //         player.position.x = (r->position.x * screen_width) + (screen_width/2.0f);
            //         player.position.y = (r->position.y * screen_height) + (screen_height/2.0f);
            //         break;
            //     }
            // }

            player.ResetPosition();

            player.key_collected = false;

            //Reset the enemy vector
            for (Enemy* e : dungeon_enemies) {
                delete e;
            }
            dungeon_enemies.clear();

            // Re-spawn enemies for the new dungeon
            DungeonEnemySpawner(dungeon_enemies, created_rooms, player, screen_width, screen_height, tile_size, tile_scale);

            player.hp = 5.0f;
            currentScreen = PLAYING;
            player.level_up = false;
            if (player.win) {
                currentScreen = WIN;
            }
            // std::cout << "!= LEVEL UP" << std::endl;
        }

        BeginDrawing();
        // std::cout << "== DRAWING" << std::endl;
        ClearBackground((Color) {0xda, 0x7a, 0x34, 0xff});
        if(currentScreen == PLAYING){
            BeginMode2D(camera_view);
        
            for (Room* r: created_rooms) {
                r->collidable_tiles.clear();
                r->collectable_tiles.clear();
                r->unlockable_tiles.clear();
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
                            
                            if (r->type == CATNIP && !r->key_collected) {
                                if (i==SCREEN_TILE_HEIGHT/2 && j==SCREEN_TILE_WIDTH/2) {
                                    tile_type = 19;
                                }
                            }

                            if (r->type == CATFOOD && !r->key_collected) {
                                if (i==SCREEN_TILE_HEIGHT/2 && j==SCREEN_TILE_WIDTH/2) {
                                    tile_type = 20;
                                }
                            }

                            // add interactions to tiles
                            // this actually allows the same tile to be in collidable_tiles multiple times
                            // at least it stops after a while
                            if (tile_types[tile_type].interactions&InteractType::COLLIDE && r->collidable_tiles.size() < SCREEN_TILE_HEIGHT*SCREEN_TILE_WIDTH) {
                                r->collidable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }
                            if (tile_types[tile_type].interactions&InteractType::COLLECT && r->collectable_tiles.size() < 1) {
                                r->collectable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }
                            if (tile_types[tile_type].interactions&InteractType::UNLOCK && r->unlockable_tiles.size() < 8) {
                                r->unlockable_tiles.push_back((Vector2){(float)(j),(float)(i)});
                            }
                            if (tile_types[tile_type].interactions&InteractType::STAIRS && r->stair_tile == ((Vector2){-1,-1})) {
                                r->stair_tile = (Vector2){(float)j,(float)i};
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
                                level_colors[player.level]
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

            if (bossEnemy->alive) {
                bossEnemy->Draw();
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
            DrawText("YOU FOUND", screen_width/2 - MeasureText("YOU FOUND", 60)/2, screen_height/2 - 110, 60, GOLD);
            DrawText("THE LIBRARIAN!", screen_width/2 - MeasureText("THE LIBRARIAN!", 60)/2, screen_height/2 - 40, 60, GOLD);
            DrawText("Press 'R' to Restart", screen_width/2 - MeasureText("Press 'R' to Restart", 30)/2, screen_height/2 + 40, 30, WHITE);

        }

        // std::cout << "!= DRAWING" << std::endl;
    
        EndDrawing();
    }

    UnloadTexture(tilemap);
    for (Enemy* e : dungeon_enemies) {
            delete e;
    }
    dungeon_enemies.clear();

    if (bossEnemy != nullptr) {
        delete bossEnemy;
        bossEnemy = nullptr;
    }

    CloseWindow();

    return 0;
}

// clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o dungeon
//  C:\raylib\w64devkit\w64devkit.exe
// HII SIRRR!! - Avielle: cd Documents/"[Y4] Second Semester 2026"/GDEV42/GDEV42 || g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm
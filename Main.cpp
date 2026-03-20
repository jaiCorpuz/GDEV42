#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

#include "Player.cpp"

using namespace std;

const int SCREEN_TILE_WIDTH = 12;
const int SCREEN_TILE_HEIGHT = 10;
// const int SCREEN_WIDTH = 768;
// const int SCREEN_HEIGHT = 640;

struct Tile {
    Rectangle source;
    bool isCollidable;

    Tile(Rectangle source, bool isCollidable) {
        this->source = source;
        this->isCollidable = isCollidable;
    }
};

enum RoomType {
    EMPTY,
    START,
    REGULAR,
    END,
    BOSS,
    DOOR,
    KEY
};

enum Direction {
    NORTH,
    WEST,
    EAST,
    SOUTH
};

struct Room;

struct Room {
    Vector2 position;
    RoomType type;
    int distance_from_start;
    vector<Room*> neighbors = {nullptr, nullptr, nullptr, nullptr};
    
    Room(vector<Room*>* rooms, Vector2 position, RoomType type) {
        this->position = position;
        this->type = type;
        distance_from_start = 0;
        // neighbor order: North, West, East, South
        (*rooms).push_back(this);
        GetNeighbors(*rooms);
    }

    Room(vector<Room*>* rooms, Room* reference, Direction direction, RoomType type) {
        int delta_x = 0;
        int delta_y = 0;
        switch (direction) {
        case NORTH:
            delta_y = -1;
            break;
        case WEST:
            delta_x = -1;
            break;
        case EAST:
            delta_x = 1;
            break;
        case SOUTH:
            delta_y = 1;
            break;
        
        default:
            break;
        }
        position = {
            reference->position.x + delta_x,
            reference->position.y + delta_y
        };
        std::cout << "creating " << position.x << " " << position.y << std::endl;
        this->type = type;
        this->distance_from_start = reference->distance_from_start + 1;
        (*rooms).push_back(this);
        GetNeighbors(*rooms);
    }

    void GetNeighbors(vector<Room*> rooms) {
        // std::cout << "for room " << this->position.x << " " << this->position.y << ":" << std::endl;
        for (Room* r : rooms) {
            // std::cout << "checking room " << r->position.x << " " << r->position.y << ":" << std::endl;
            if (this->position.x - r->position.x == -1 && this->position.y == r->position.y) {
                // std::cout << "Found EAST:" << std::endl;
                this->neighbors.at(EAST) = r;
                r->neighbors.at(WEST) = this;
            } else if (this->position.x - r->position.x == 1 && this->position.y == r->position.y) {
                // std::cout << "Found WEST:" << std::endl;
                this->neighbors.at(WEST) = r;
                r->neighbors.at(EAST) = this;
            } else if (this->position.y - r->position.y == -1 && this->position.x == r->position.x) {
                // std::cout << "Found SOUTH:" << std::endl;
                this->neighbors.at(SOUTH) = r;
                r->neighbors.at(NORTH) = this;
            } else if (this->position.y - r->position.y == 1 && this->position.x == r->position.x) {
                // std::cout << "Found NORTH:" << std::endl;
                this->neighbors.at(NORTH) = r;
                r->neighbors.at(SOUTH) = this;
            }
        }
    }
    
    int CountOccupiedNeighbors() {
        int count = 0;
        for (Room* r : neighbors) {
            if (r != nullptr && r->type != EMPTY) {
                count++;
            }
        }
        return count;
    }

    vector<Room*> FindEndRooms(vector<Room*> rooms) {
        vector<Room*> end_rooms;
        for (Room* r: rooms) {
            if (r->type == END || r->type == START) {
                end_rooms.push_back(r);
            }
        }
        return end_rooms;
    }
};

vector<Room*> GenerateDungeon() {
    int target_rooms = 5 + rand() % 6;
    vector<Room*> rooms;
    
    new Room(&rooms, {0, 0}, START);
    
    Room* start_room = rooms.at(0);
    
    vector<Room*> created_rooms;
    queue<Room*> q;
    created_rooms.push_back(start_room);
    q.push(start_room);
    
    int room_count = 1;
    
    while (room_count < target_rooms) {
        Room* current_room = q.front();
        q.pop();
        std::cout << "current room " << current_room->position.x << " " << current_room->position.y << ":" << std::endl;
        
        bool created_room = false;
        
        for (int i = 0; i < 4; i++) {
            if (current_room->neighbors.at(i) == nullptr) {
                new Room(
                    &rooms,
                    current_room,
                    (Direction) i,
                    EMPTY
                );
            }
        }
        std::cout << "== after creation ==" << std::endl;
        for (int i = 0; i < 4; i++) {
            if (current_room->neighbors.at(i) == nullptr) {
                std::cout << "nullptr" << std::endl;
            } else {
                std::cout << "room" << std::endl;
            }
        }
        std::cout << "== after check ==" << std::endl;
        for (int i = 0; i < 4; i++) {
            Room* n = current_room->neighbors.at(i);
            std::cout << "current neighbor " << n->position.x << " " << n->position.y << std::endl;
            if (n->type != EMPTY) continue;
            if (n->CountOccupiedNeighbors() > 1) continue;
            if (rand() % 2 == 0) {
                n->type = REGULAR;
                room_count++;
                created_room = true;
                std::cout << "creating room " << n->position.x << " " << n->position.y << ":" << std::endl;
                
                if (current_room->type == END) {
                    current_room->type = REGULAR;
                    n->distance_from_start = current_room->distance_from_start + 1;
                }
                
                created_rooms.push_back(n);
                q.push(n);
                
                if (room_count >= target_rooms) break;
            }
        }
        
        if (!created_room && current_room->type != START) {
            current_room->type = END;
            if (current_room->CountOccupiedNeighbors() == 1) {
                current_room->type = END;
            }
        }
        
        if (q.empty()) {
            std::cout << "queue empty, " << room_count << " out of " << target_rooms << " created..." << std::endl;
            vector<Room*> end_rooms = start_room->FindEndRooms(rooms);
            for (Room* r: end_rooms) {
                q.push(r);
            }
        }
    }
    std::cout << "loop exited, " << room_count << " out of " << target_rooms << " created..." << std::endl;

    int max_distance = -1;
    Room* current_boss_room = nullptr;
    
    for (Room* r: created_rooms) {
        if (r->CountOccupiedNeighbors() == 1 && r->type != START) {
            r->type = END;
        }
    }
    for (Room* r: created_rooms) {
        if (r->type == END && r->distance_from_start > max_distance) {
            current_boss_room = r;
            max_distance = r->distance_from_start;
        }    
    }
    current_boss_room->type = BOSS;
    for (Room* r: created_rooms) {
        std::cout << r->CountOccupiedNeighbors() << " room " << r->position.x << " " << r->position.y;
        switch (r->type)
        {
            case START:
            std::cout << " start ";
            break;
            case END:
            std::cout << " end ";
            break;
            case REGULAR:
            std::cout << " regular ";
            break;
            case BOSS:
            std::cout << " boss ";
            break;
        
        default:
            break;
        }
        std::cout << r->distance_from_start;

        std::cout << std::endl;
    }
    
    current_boss_room->type = BOSS;

    return created_rooms;
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

    Player player({0,0}, 25, 200.0f);

    int cam_type = 0;
    Camera2D camera_view ={0};
    camera_view.target = player.position;
    camera_view.offset = {(float) screen_width /2 , (float)screen_height / 2};
    camera_view.zoom = 1.0f;
    
    vector<Room*> created_rooms = GenerateDungeon();
    
    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();
        
        camera_view.target = player.position;
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
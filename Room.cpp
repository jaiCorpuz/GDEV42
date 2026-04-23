#include <raylib.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Tile.cpp"
#include "Enemy.hpp"

enum RoomType {
    EMPTY,
    START,
    REGULAR,
    END,
    BOSS,
    DOOR,
    KEY,
    CATFOOD, 
    CATNIP
};

enum Direction {
    NORTH,
    WEST,
    EAST,
    SOUTH
};

const int SCREEN_TILE_WIDTH = 12;
const int SCREEN_TILE_HEIGHT = 10;
int screen_width;
int screen_height;

const bool ROOM_OUT = false;
const bool BOSS_OUT = false;
const bool DOOR_OUT = false;
const bool KEY_OUT = false;

struct Room;

struct Room {
    Vector2 position;
    RoomType type;
    int distance_from_start;
    bool is_locked = false;
    bool key_collected = false;
    std::vector<Vector2> collidable_tiles;
    std::vector<Vector2> collectable_tiles;
    std::vector<Vector2> unlockable_tiles;
    Vector2 stair_tile = {-1, -1};
    std::vector<Room*> neighbors = {nullptr, nullptr, nullptr, nullptr};
    std::vector<Enemy*> enemies;
    
    Room(std::vector<Room*>* rooms, Vector2 position, RoomType type) {
        this->position = position;
        this->type = type;
        distance_from_start = 0;
        // neighbor order: North, West, East, South
        (*rooms).push_back(this);
        GetNeighbors(*rooms);
    }

    Room(std::vector<Room*>* rooms, Room* reference, Direction direction, RoomType type) {
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
        if (ROOM_OUT) std::cout << "creating " << position.x << " " << position.y << std::endl;
        this->type = type;
        this->distance_from_start = reference->distance_from_start + 1;
        (*rooms).push_back(this);
        GetNeighbors(*rooms);
    }

    void GetNeighbors(std::vector<Room*> rooms) {
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

    std::vector<Room*> FindEndRooms(std::vector<Room*> rooms) {
        std::vector<Room*> end_rooms;
        for (Room* r: rooms) {
            if (r->type == END || r->type == START) {
                end_rooms.push_back(r);
            }
        }
        return end_rooms;
    }
};

std::vector<Room*> GenerateDungeon(int target) {
    // int target_rooms;
    // if (min == max) {
    //     target_rooms = min;
    // } else {
    //     target_rooms = min + rand() % (max - min + 1);
    // }

    int target_rooms = target;
    std::vector<Room*> rooms;
    
    new Room(&rooms, {0, 0}, START);
    
    Room* start_room = rooms.at(0);
    
    std::vector<Room*> created_rooms;
    std::queue<Room*> q;
    created_rooms.push_back(start_room);
    created_rooms.at(0)->type = START;
    q.push(start_room);
    
    int room_count = 1;
    
    while (room_count < target_rooms) {
        Room* current_room = q.front();
        q.pop();
        if (ROOM_OUT) std::cout << "current room " << current_room->position.x << " " << current_room->position.y << ":" << std::endl;
        
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
        if (ROOM_OUT) std::cout << "== after creation ==" << std::endl;
        for (int i = 0; i < 4; i++) {
            if (current_room->neighbors.at(i) == nullptr) {
                if (ROOM_OUT) std::cout << "nullptr" << std::endl;
            } else {
                if (ROOM_OUT) std::cout << "room" << std::endl;
            }
        }
        if (ROOM_OUT) std::cout << "== after check ==" << std::endl;
        for (int i = 0; i < 4; i++) {
            Room* n = current_room->neighbors.at(i);
            if (ROOM_OUT) std::cout << "current neighbor " << n->position.x << " " << n->position.y << std::endl;
            if (n->type != EMPTY) continue;
            if (n->CountOccupiedNeighbors() > 1) continue;
            if (rand() % 2 == 0) {
                n->type = REGULAR;
                room_count++;
                created_room = true;
                if (ROOM_OUT) std::cout << "creating room " << n->position.x << " " << n->position.y << ":" << std::endl;
                
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
            if (ROOM_OUT) std::cout << "queue empty, " << room_count << " out of " << target_rooms << " created..." << std::endl;
            std::vector<Room*> end_rooms = start_room->FindEndRooms(rooms);
            for (Room* r: end_rooms) {
                q.push(r);
            }
        }
    }
    if (ROOM_OUT) std::cout << "loop exited, " << room_count << " out of " << target_rooms << " created..." << std::endl;

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
    if (BOSS_OUT) {
        for (Room* r: created_rooms) {
            if (ROOM_OUT) std::cout << r->CountOccupiedNeighbors() << " room " << r->position.x << " " << r->position.y;
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
    }
    
    current_boss_room->type = BOSS;
    // std::cout << TextFormat("boss room set: %.0f %.0f", current_boss_room->position.x, current_boss_room->position.y) << std::endl;
    
    int current_distance_from_start = current_boss_room->distance_from_start;
    // std::cout << TextFormat("curr: %d", current_distance_from_start) << std::endl;
    int door_rand = rand();
    int door_min;
    int door_max;
    int door_distance;
    if (created_rooms.at(0)->CountOccupiedNeighbors() == 1) {
        door_min = 2;
        door_max = current_boss_room->distance_from_start-1;
    } else {
        door_min = 1;
        door_max = current_boss_room->distance_from_start;
    }
    door_distance = door_min + door_rand % door_max;
    if (DOOR_OUT) std::cout << TextFormat("door: %d < %d < %d ", door_min, door_distance, door_max) << std::endl;
    Room* current_door_room = current_boss_room;
    // if (DOOR_OUT) std::cout << TextFormat("current door room: %.0f %.0f, dist: %d", current_door_room->position.x, current_door_room->position.y, current_door_room->distance_from_start) << std::endl;
    std::vector<Room*> door_visited_rooms;
    door_visited_rooms.push_back(current_door_room);
    
    // if (DOOR_OUT) std::cout << TextFormat("curr %d, goal %d", current_distance_from_start, door_distance) << std::endl;
    bool door_searching = true;
    while (door_searching) {
        if (current_distance_from_start == door_distance) {
            if (DOOR_OUT) std::cout << TextFormat("final door room: %.0f %.0f, dist: %d", current_door_room->position.x, current_door_room->position.y, current_door_room->distance_from_start) << std::endl;
            if (DOOR_OUT) std::cout << "door search done" << std::endl;
            door_searching = false;
            break;
        }
        // if (DOOR_OUT) std::cout << TextFormat("current door room: %.0f %.0f, dist: %d", current_door_room->position.x, current_door_room->position.y, current_door_room->distance_from_start) << std::endl;
        // if (DOOR_OUT) std::cout << TextFormat("neighbors %d", current_door_room->CountOccupiedNeighbors()) << std::endl;
        for (Room* n : current_door_room->neighbors) {
            if (n != nullptr && n->type != EMPTY) {
                // if (DOOR_OUT) std::cout << TextFormat("current neighbor: %.0f %.0f, dist: %d", n->position.x, n->position.y, n->distance_from_start) << std::endl;
                if (n->distance_from_start < current_distance_from_start && n->distance_from_start != 0) {
                    current_distance_from_start = n->distance_from_start;
                    current_door_room = n;
                    door_visited_rooms.push_back(current_door_room);
                    break;
                }
            }
        }
    }
    current_door_room->is_locked = true;
    
    bool key_room_search = true;
    Room* current_key_room = created_rooms.at(0);
    std::vector<Room*> ks_visited;
    std::queue<Room*> ks_not_visited;
    ks_visited.push_back(current_door_room);
    ks_not_visited.push(current_key_room);
    
    if (KEY_OUT) std::cout << "=== key search START" << std::endl;
    while(!ks_not_visited.empty()) {
        current_key_room = ks_not_visited.front();
        ks_not_visited.pop();
        ks_visited.push_back(current_key_room);
        if (KEY_OUT) std::cout << TextFormat("visited room: %.0f %.0f", current_key_room->position.x, current_key_room->position.y) << std::endl;
        for (Room* n : current_key_room->neighbors) {
            if (n == nullptr || n->type == EMPTY || n->is_locked) {
                // if (KEY_OUT) std::cout << TextFormat("skipped room: %.0f %.0f", n->position.x, n->position.y) << std::endl;
                continue;
            } else {
                bool visited = false;
                for (Room* r: ks_visited) {
                    if (
                        n->position.x == r->position.x &&
                        n->position.y == r->position.y
                    ) {
                        visited = true;
                        break;
                    }
                }
                if (!visited && n->distance_from_start != 0) {
                    if (KEY_OUT) std::cout << TextFormat("not visited room: %.0f %.0f", n->position.x, n->position.y) << std::endl;
                    ks_visited.push_back(n);
                    ks_not_visited.push(n);
                }
            }
        }
    }
    if (KEY_OUT) std::cout << "=== key search CANDIDATES" << std::endl;
    std::vector<Room*> key_room_candidates;
    for (Room* r: ks_visited) {
        if (KEY_OUT) std::cout << TextFormat("- checking room: %.0f %.0f", r->position.x, r->position.y) << std::endl;
        bool added = false;
        for (Room* k: key_room_candidates) {
            if (
                k->position.x == r->position.x &&
                k->position.y == r->position.y
            ) {
                if (KEY_OUT) std::cout << "already a candidate" << std::endl;
                added = true;
            }
        }
        if (
            !added &&
            (r->type != DOOR && r->type != START && r->type != EMPTY) &&
            !r->is_locked
        ) {
            if (KEY_OUT) std::cout << TextFormat("- candidate room: %.0f %.0f", r->position.x, r->position.y) << std::endl;
            key_room_candidates.push_back(r);
        }
    }
    
    int key_room_index = rand() % key_room_candidates.size();
    if (KEY_OUT) std::cout << "=== key search DONE" << std::endl;
    key_room_candidates.at(key_room_index)->type = KEY;
    std::cout << TextFormat("key room: %.0f %.0f", key_room_candidates.at(key_room_index)->position.x, key_room_candidates.at(key_room_index)->position.y) << std::endl;

    std::vector<Room*> validCandidateRooms;
    for (Room* r : created_rooms) {
        if (r->type == REGULAR) {
            validCandidateRooms.push_back(r);
        }
    }
    
    for (int i =validCandidateRooms.size() - 1; i > 0; --i) {
        int j = rand() % (i+1);
        std::swap(validCandidateRooms[i], validCandidateRooms[j]);
    }

    if (validCandidateRooms.size() >= 2) {
        validCandidateRooms[0]->type = CATFOOD;
        validCandidateRooms[1]->type = CATNIP;
    }
    
    return created_rooms;
}


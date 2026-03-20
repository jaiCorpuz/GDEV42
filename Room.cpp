#include <raylib.h>
#include <vector>
#include <queue>
#include <iostream>

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
    std::vector<Vector2> collidable_tiles;
    std::vector<Room*> neighbors = {nullptr, nullptr, nullptr, nullptr};
    
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
        std::cout << "creating " << position.x << " " << position.y << std::endl;
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

std::vector<Room*> GenerateDungeon() {
    int target_rooms = 5 + rand() % 6;
    std::vector<Room*> rooms;
    
    new Room(&rooms, {0, 0}, START);
    
    Room* start_room = rooms.at(0);
    
    std::vector<Room*> created_rooms;
    std::queue<Room*> q;
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
            std::vector<Room*> end_rooms = start_room->FindEndRooms(rooms);
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
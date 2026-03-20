#include <raylib.h>

struct Tile {
    Rectangle source;
    bool isCollidable;

    Tile(Rectangle source, bool isCollidable) {
        this->source = source;
        this->isCollidable = isCollidable;
    }
};

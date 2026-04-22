#include <raylib.h>

enum InteractType : unsigned int {
    COLLIDE = 1 << 0,
    COLLECT = 1 << 1,
    UNLOCK  = 1 << 2,
    STAIRS  = 1 << 3
};

struct Tile {
    static int size;
    static int scale;

    Rectangle source;
    unsigned int interactions;

    Tile(
        Rectangle source,
        unsigned int interactions
    ) {
        this->source = source;
        this->interactions = interactions;
    }
};

#include <raylib.h>

struct Tile {
    static int size;
    static int scale;

    Rectangle source;
    bool isCollidable;
    bool isCollectable;
    bool isUnlockable;

    Tile(
        Rectangle source,
        bool isCollidable,
        bool isCollectable,
        bool isUnlockable
    ) {
        this->source = source;
        this->isCollidable = isCollidable;
        this->isCollectable = isCollectable;
        this->isUnlockable = isUnlockable;
    }
};

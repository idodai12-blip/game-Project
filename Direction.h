#pragma once
#include "Point.h"

enum class Direction {
    NONE = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

inline Point directionToPoint(Direction dir) {
    switch (dir) {
        case Direction::UP: return Point(0, -1);
        case Direction::DOWN: return Point(0, 1);
        case Direction::LEFT: return Point(-1, 0);
        case Direction::RIGHT: return Point(1, 0);
        default: return Point(0, 0);
    }
}

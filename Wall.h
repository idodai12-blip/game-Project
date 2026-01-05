#pragma once
#include "GameElement.h"

class Wall : public GameElement {
public:
    Wall(Point pos) : GameElement(pos, 'W') {}
    
    bool canPlayerPass() const override { return false; }
};

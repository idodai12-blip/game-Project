#pragma once
#include "GameElement.h"

class Obstacle : public GameElement {
public:
    Obstacle(Point pos) : GameElement(pos, '*') {}
    
    bool canPlayerPass() const override { return false; }
    
    // Obstacles can be pushed
    bool canBePushed() const { return true; }
};

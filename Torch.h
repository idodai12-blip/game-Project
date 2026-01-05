#pragma once
#include "GameElement.h"

class Torch : public GameElement {
public:
    Torch(Point pos) : GameElement(pos, '!') {}
    
    bool canPlayerPass() const override { return true; }
    bool isCollectible() const override { return true; }
};

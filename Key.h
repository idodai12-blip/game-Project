#pragma once
#include "GameElement.h"

class Key : public GameElement {
public:
    Key(Point pos) : GameElement(pos, 'K') {}
    
    bool canPlayerPass() const override { return true; }
    bool isCollectible() const override { return true; }
};

#pragma once
#include "GameElement.h"

class Bomb : public GameElement {
private:
    int ticksRemaining;
    bool activated;
    
public:
    Bomb(Point pos) : GameElement(pos, '@'), ticksRemaining(-1), activated(false) {}
    
    bool canPlayerPass() const override { return true; }
    bool isCollectible() const override { return !activated; }
    
    void activate() {
        activated = true;
        ticksRemaining = 5;
    }
    
    bool isActivated() const { return activated; }
    
    // Returns true if bomb exploded
    bool tick() {
        if (!activated) return false;
        ticksRemaining--;
        return ticksRemaining <= 0;
    }
};

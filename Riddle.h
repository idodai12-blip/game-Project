#pragma once
#include "GameElement.h"
#include <string>

class Riddle : public GameElement {
private:
    bool active;  // Whether riddle overlay is currently showing
    
public:
    Riddle(Point pos) : GameElement(pos, '?'), active(false) {}
    
    bool canPlayerPass() const override { return true; }  // Player can pass through after solving
    
    bool isActive() const { return active; }
    void setActive(bool isActive) { active = isActive; }
};

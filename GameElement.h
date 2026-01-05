#pragma once
#include "Point.h"

// Base class for all game elements
class GameElement {
protected:
    Point position;
    char displayChar;
    
public:
    GameElement(Point pos, char ch) : position(pos), displayChar(ch) {}
    virtual ~GameElement() = default;
    
    Point getPosition() const { return position; }
    void setPosition(Point pos) { position = pos; }
    char getDisplayChar() const { return displayChar; }
    
    virtual void draw() const;
    virtual bool canPlayerPass() const = 0;  // Pure virtual - must implement
    virtual bool isCollectible() const { return false; }
};

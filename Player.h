#pragma once
#include "Point.h"
#include "Direction.h"
#include "GameElement.h"

class Player {
private:
    Point position;
    Direction direction;
    char symbol;
    GameElement* heldItem;  // Non-owning pointer
    
public:
    Player(Point pos, char sym);
    
    Point getPosition() const { return position; }
    void setPosition(Point pos) { position = pos; }
    
    Direction getDirection() const { return direction; }
    void setDirection(Direction dir) { direction = dir; }
    void stop() { direction = Direction::NONE; }
    
    Point getNextPosition() const { return position + directionToPoint(direction); }
    
    char getSymbol() const { return symbol; }
    
    bool hasItem() const { return heldItem != nullptr; }
    GameElement* getHeldItem() const { return heldItem; }
    void pickUpItem(GameElement* item) { heldItem = item; }
    GameElement* disposeItem();
    
    void draw() const;
};

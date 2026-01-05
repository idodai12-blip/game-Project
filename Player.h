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
    
    // Spring acceleration state
    Direction springDirection;
    int springVelocity;
    int springCyclesRemaining;
    
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
    
    // Spring acceleration methods
    bool isUnderSpringEffect() const { return springCyclesRemaining > 0; }
    Direction getSpringDirection() const { return springDirection; }
    int getSpringVelocity() const { return springVelocity; }
    void setSpringEffect(Direction dir, int velocity, int cycles) {
        springDirection = dir;
        springVelocity = velocity;
        springCyclesRemaining = cycles;
    }
    void clearSpringEffect() {
        springVelocity = 0;
        springCyclesRemaining = 0;
        springDirection = Direction::NONE;
    }
    void decrementSpringCycles() {
        if (springCyclesRemaining > 0) {
            springCyclesRemaining--;
        }
    }
    
    void draw() const;
};

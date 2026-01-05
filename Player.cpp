#include "Player.h"
#include "GameConfig.h"
#include <iostream>

Player::Player(Point pos, char sym) 
    : position(pos), direction(Direction::NONE), symbol(sym), heldItem(nullptr) {}

GameElement* Player::disposeItem() {
    GameElement* item = heldItem;
    heldItem = nullptr;
    return item;
}

void Player::draw() const {
    gotoxy(position.getX(), position.getY());
    std::cout << symbol;
}

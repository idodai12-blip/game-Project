#include "GameElement.h"
#include "GameConfig.h"
#include <iostream>

void GameElement::draw() const {
    gotoxy(position.getX(), position.getY() + SCREEN_OFFSET_Y);
    std::cout << displayChar;
}

#pragma once
#include "GameElement.h"

class Door : public GameElement {
private:
    int doorNumber;
    int targetRoomId;
    
public:
    Door(Point pos, int number, int targetRoom) 
        : GameElement(pos, '0' + number), doorNumber(number), targetRoomId(targetRoom) {}
    
    bool canPlayerPass() const override { return true; }
    int getDoorNumber() const { return doorNumber; }
    int getTargetRoomId() const { return targetRoomId; }
};

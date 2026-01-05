#pragma once
#include "GameElement.h"

class Switch;  // Forward declaration

class Door : public GameElement {
private:
    int doorNumber;
    int targetRoomId;
    int switchGroupId;  // -1 means no switches required, >=0 means requires switches
    
public:
    Door(Point pos, int number, int targetRoom, int switchGroup = -1) 
        : GameElement(pos, '0' + number), doorNumber(number), 
          targetRoomId(targetRoom), switchGroupId(switchGroup) {}
    
    bool canPlayerPass() const override { return true; }
    int getDoorNumber() const { return doorNumber; }
    int getTargetRoomId() const { return targetRoomId; }
    int getSwitchGroupId() const { return switchGroupId; }
};

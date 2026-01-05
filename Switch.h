#pragma once
#include "GameElement.h"

class Switch : public GameElement {
private:
    bool isOn;
    int groupId;  // Which door group this switch belongs to
    
public:
    Switch(Point pos, int group) : GameElement(pos, '\\'), isOn(false), groupId(group) {}
    
    bool canPlayerPass() const override { return true; }
    
    bool getIsOn() const { return isOn; }
    void toggle() { 
        isOn = !isOn; 
        displayChar = isOn ? '/' : '\\';  // Visual feedback
    }
    
    int getGroupId() const { return groupId; }
};

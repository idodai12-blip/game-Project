#include "Room.h"
#include "GameConfig.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Room::Room(int id, bool finalRoom)
    : roomId(id), isFinalRoom(finalRoom) {}

void Room::addElement(std::unique_ptr<GameElement> element) {
    // Store raw pointer BEFORE moving ownership
    GameElement* rawPtr = element.get();
    
    // Move ownership to elements vector
    elements.push_back(std::move(element));
    
    // Now store non-owning pointers in quick-access lists
    if (Door* door = dynamic_cast<Door*>(rawPtr)) {
        doors.push_back(door);
    } else if (Bomb* bomb = dynamic_cast<Bomb*>(rawPtr)) {
        bombs.push_back(bomb);
    } else if (Obstacle* obs = dynamic_cast<Obstacle*>(rawPtr)) {
        obstacles.push_back(obs);
    } else if (Riddle* riddle = dynamic_cast<Riddle*>(rawPtr)) {
        riddles.push_back(riddle);
    } else if (Switch* sw = dynamic_cast<Switch*>(rawPtr)) {
        switches.push_back(sw);
    } else if (Spring* spring = dynamic_cast<Spring*>(rawPtr)) {
        springs.push_back(spring);
    }
}

GameElement* Room::getElementAt(Point pos) const {
    for (const auto& elem : elements) {
        if (elem && elem->getPosition() == pos) {
            // Check if element is on-screen (not collected)
            Point elemPos = elem->getPosition();
            if (elemPos.getX() >= 0 && elemPos.getX() < SCREEN_WIDTH &&
                elemPos.getY() >= 0 && elemPos.getY() < SCREEN_HEIGHT) {
                return elem.get();
            }
        }
    }
    return nullptr;
}

// NEW METHOD - instead of actually removing, we just hide collected items
void Room::markElementAsCollected(GameElement* element) {
    // Move element off-screen instead of deleting
    element->setPosition(Point(-100, -100));
}

bool Room::isPositionWalkable(Point pos) const {
    if (pos.getX() < 0 || pos.getX() >= SCREEN_WIDTH ||
        pos.getY() < 0 || pos.getY() >= SCREEN_HEIGHT) {
        return false;
    }
    
    GameElement* elem = getElementAt(pos);
    if (elem) {
        return elem->canPlayerPass();
    }
    
    return true;
}

bool Room::isWall(Point pos) const {
    GameElement* elem = getElementAt(pos);
    return dynamic_cast<Wall*>(elem) != nullptr;
}

Door* Room::getDoorAt(Point pos) const {
    for (Door* door : doors) {
        if (door && door->getPosition() == pos) {
            // Check if door is on-screen (not collected/destroyed)
            Point doorPos = door->getPosition();
            if (doorPos.getX() >= 0 && doorPos.getX() < SCREEN_WIDTH &&
                doorPos.getY() >= 0 && doorPos.getY() < SCREEN_HEIGHT) {
                return door;
            }
        }
    }
    return nullptr;
}

Obstacle* Room::getObstacleAt(Point pos) const {
    for (Obstacle* obs : obstacles) {
        if (obs && obs->getPosition() == pos) {
            // Check if obstacle is on-screen (not collected/destroyed)
            Point obsPos = obs->getPosition();
            if (obsPos.getX() >= 0 && obsPos.getX() < SCREEN_WIDTH &&
                obsPos.getY() >= 0 && obsPos.getY() < SCREEN_HEIGHT) {
                return obs;
            }
        }
    }
    return nullptr;
}

Riddle* Room::getRiddleAt(Point pos) const {
    for (Riddle* riddle : riddles) {
        if (riddle && riddle->getPosition() == pos) {
            // Check if riddle is on-screen (not solved)
            Point riddlePos = riddle->getPosition();
            if (riddlePos.getX() >= 0 && riddlePos.getX() < SCREEN_WIDTH &&
                riddlePos.getY() >= 0 && riddlePos.getY() < SCREEN_HEIGHT) {
                return riddle;
            }
        }
    }
    return nullptr;
}

Switch* Room::getSwitchAt(Point pos) const {
    for (Switch* sw : switches) {
        if (sw && sw->getPosition() == pos) {
            // Check if switch is on-screen (not destroyed)
            Point switchPos = sw->getPosition();
            if (switchPos.getX() >= 0 && switchPos.getX() < SCREEN_WIDTH &&
                switchPos.getY() >= 0 && switchPos.getY() < SCREEN_HEIGHT) {
                return sw;
            }
        }
    }
    return nullptr;
}

bool Room::areSwitchesActivated(int groupId) const {
    if (groupId < 0) return true;  // No switches required
    
    // Check if all switches in this group are ON
    bool foundAny = false;
    for (Switch* sw : switches) {
        if (sw && sw->getGroupId() == groupId) {
            foundAny = true;
            Point switchPos = sw->getPosition();
            // Only count switches that are still on-screen
            if (switchPos.getX() >= 0 && switchPos.getX() < SCREEN_WIDTH &&
                switchPos.getY() >= 0 && switchPos.getY() < SCREEN_HEIGHT) {
                if (!sw->getIsOn()) {
                    return false;  // Found an OFF switch
                }
            }
        }
    }
    
    return foundAny;  // All switches are ON (or no switches in group)
}

Spring* Room::getSpringAt(Point pos) const {
    for (Spring* spring : springs) {
        if (spring && spring->isPartOfSpring(pos)) {
            // Check if spring is on-screen (not destroyed)
            Point springPos = spring->getPosition();
            if (springPos.getX() >= 0 && springPos.getX() < SCREEN_WIDTH &&
                springPos.getY() >= 0 && springPos.getY() < SCREEN_HEIGHT) {
                return spring;
            }
        }
    }
    return nullptr;
}

bool Room::tryPushObstacle(Obstacle* obs, Direction dir) {
    if (!obs) return false;
    
    Point newPos = obs->getPosition() + directionToPoint(dir);
    
    // Check if new position is valid and empty
    if (!isPositionWalkable(newPos)) {
        return false;
    }
    
    // Check if there's already something there
    if (getElementAt(newPos) != nullptr) {
        return false;
    }
    
    // Push the obstacle
    obs->setPosition(newPos);
    return true;
}

void Room::updateBombs() {
    for (auto it = bombs.begin(); it != bombs.end(); ) {
        if (*it && (*it)->isActivated() && (*it)->tick()) {
            explodeBomb(*it);
            // Remove from quick-access list only
            it = bombs.erase(it);
        } else {
            ++it;
        }
    }
}

void Room::explodeBomb(Bomb* bomb) {
    if (!bomb) return;
    
    Point bombPos = bomb->getPosition();
    
    // Remove bomb itself
    markElementAsCollected(bomb);
    
    // Destroy adjacent walls (distance <= 1)
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            
            Point checkPos(bombPos.getX() + dx, bombPos.getY() + dy);
            GameElement* elem = getElementAt(checkPos);
            
            if (elem && dynamic_cast<Wall*>(elem)) {
                markElementAsCollected(elem);
            }
        }
    }
    
    // Destroy other objects within radius 3
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx == 0 && dy == 0) continue;
            
            Point checkPos(bombPos.getX() + dx, bombPos.getY() + dy);
            GameElement* elem = getElementAt(checkPos);
            
            if (elem && !dynamic_cast<Wall*>(elem)) {
                markElementAsCollected(elem);
            }
        }
    }
}

void Room::draw() const {
    for (const auto& elem : elements) {
        if (elem) {
            // Don't draw elements that are off-screen (collected)
            Point pos = elem->getPosition();
            if (pos.getX() >= 0 && pos.getX() < SCREEN_WIDTH &&
                pos.getY() >= 0 && pos.getY() < SCREEN_HEIGHT) {
                elem->draw();
            }
        }
    }
}

void Room::drawLegend(Player* p1, Player* p2, int x, int y, int lives, int score) const {
    gotoxy(x, y);
    std::cout << "P1: ";
    if (p1->hasItem()) {
        std::cout << p1->getHeldItem()->getDisplayChar();
    } else {
        std::cout << "-";
    }
    
    gotoxy(x + 10, y);
    std::cout << "P2: ";
    if (p2->hasItem()) {
        std::cout << p2->getHeldItem()->getDisplayChar();
    } else {
        std::cout << "-";
    }
    
    gotoxy(x, y + 1);
    std::cout << "Life: " << lives;
    
    gotoxy(x, y + 2);
    std::cout << "Score: " << score;
}

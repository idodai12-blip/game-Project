#pragma once
#include "Point.h"
#include "GameElement.h"
#include "Wall.h"
#include "Key.h"
#include "Door.h"
#include "Torch.h"
#include "Bomb.h"
#include "Obstacle.h"
#include "Riddle.h"
#include "Switch.h"
#include "Spring.h"
#include "Player.h"
#include <vector>
#include <memory>

class Room {
private:
    int roomId;
    bool isFinalRoom;
    std::vector<std::unique_ptr<GameElement>> elements;
    
    // Quick access lists (non-owning pointers)
    std::vector<Door*> doors;
    std::vector<Bomb*> bombs;
    std::vector<Obstacle*> obstacles;
    std::vector<Riddle*> riddles;
    std::vector<Switch*> switches;
    std::vector<Spring*> springs;
    
public:
    Room(int id, bool finalRoom = false);
    
    // Prevent copying
    Room(const Room&) = delete;
    Room& operator=(const Room&) = delete;
    
    int getId() const { return roomId; }
    bool getIsFinalRoom() const { return isFinalRoom; }
    
    void addElement(std::unique_ptr<GameElement> element);
    GameElement* getElementAt(Point pos) const;
    void markElementAsCollected(GameElement* element);  // NEW - instead of removeElement
    
    bool isPositionWalkable(Point pos) const;
    bool isWall(Point pos) const;
    Door* getDoorAt(Point pos) const;
    Obstacle* getObstacleAt(Point pos) const;
    Riddle* getRiddleAt(Point pos) const;
    Switch* getSwitchAt(Point pos) const;
    Spring* getSpringAt(Point pos) const;
    
    bool areSwitchesActivated(int groupId) const;
    
    void updateBombs();
    void explodeBomb(Bomb* bomb);
    bool tryPushObstacle(Obstacle* obs, Direction dir);
    
    // Get all springs (for collapsing all springs at once)
    const std::vector<Spring*>& getSprings() const { return springs; }
    
    void draw() const;
    void drawLegend(Player* p1, Player* p2, int x, int y, int lives, int score) const;
};

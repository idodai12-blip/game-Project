#pragma once
#include "Player.h"
#include "Room.h"
#include <vector>
#include <memory>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    EXIT
};

class Game {
private:
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    std::vector<std::unique_ptr<Room>> rooms;
    std::vector<Point> legendPositions;  // Legend position for each room
    int currentRoomIndex;
    bool player1ReachedEnd;
    bool player2ReachedEnd;
    GameState state;
    
    void loadRoomsFromFiles();
    void handlePlayerInput(Player* player, char key);  // Unified for both players
    void updatePlayer(Player* player, Player* otherPlayer);  // Unified for both players
    void checkCollisions();
    void checkDoors();
    void drawGame();
    void showMenu();
    void showInstructions();
    void pauseGame();
    
public:
    Game();
    
    // Prevent copying (as requested by grader)
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    
    void run();
    void startNewGame();
    Room* getCurrentRoom() { return rooms[currentRoomIndex].get(); }
};

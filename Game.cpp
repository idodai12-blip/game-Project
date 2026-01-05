#include "Game.h"
#include "GameConfig.h"
#include "Wall.h"
#include "Key.h"
#include "Door.h"
#include "Torch.h"
#include "Bomb.h"
#include "Obstacle.h"
#include <iostream>

Game::Game() 
    : currentRoomIndex(0), player1ReachedEnd(false), player2ReachedEnd(false), state(GameState::MENU) {
    player1 = std::make_unique<Player>(Point(5, 10), Chars::PLAYER1);
    player2 = std::make_unique<Player>(Point(5, 12), Chars::PLAYER2);
    createRooms();
}

void Game::createRooms() {
    // Room 1
    auto room1 = std::make_unique<Room>(1);
    
    // Borders
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        room1->addElement(std::make_unique<Wall>(Point(x, 0)));
        room1->addElement(std::make_unique<Wall>(Point(x, SCREEN_HEIGHT - 1)));
    }
    for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
        room1->addElement(std::make_unique<Wall>(Point(0, y)));
        room1->addElement(std::make_unique<Wall>(Point(SCREEN_WIDTH - 1, y)));
    }
    
    // Internal walls
    for (int y = 5; y < 15; y++) {
        room1->addElement(std::make_unique<Wall>(Point(20, y)));
    }
    
    // Items
    room1->addElement(std::make_unique<Torch>(Point(10, 7)));
    room1->addElement(std::make_unique<Key>(Point(25, 10)));
    room1->addElement(std::make_unique<Obstacle>(Point(15, 10)));
    room1->addElement(std::make_unique<Door>(Point(60, 12), 1, 2));
    
    rooms.push_back(std::move(room1));
    
    // Room 2
    auto room2 = std::make_unique<Room>(2);
    
    // Borders
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        room2->addElement(std::make_unique<Wall>(Point(x, 0)));
        room2->addElement(std::make_unique<Wall>(Point(x, SCREEN_HEIGHT - 1)));
    }
    for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
        room2->addElement(std::make_unique<Wall>(Point(0, y)));
        room2->addElement(std::make_unique<Wall>(Point(SCREEN_WIDTH - 1, y)));
    }
    
    // Walls that can be destroyed
    for (int x = 30; x < 35; x++) {
        room2->addElement(std::make_unique<Wall>(Point(x, 12)));
    }
    
    // Items
    room2->addElement(std::make_unique<Bomb>(Point(20, 12)));
    room2->addElement(std::make_unique<Key>(Point(40, 12)));
    room2->addElement(std::make_unique<Door>(Point(70, 12), 2, 3));
    
    rooms.push_back(std::move(room2));
    
    // Room 3
    auto room3 = std::make_unique<Room>(3);
    
    // Borders
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        room3->addElement(std::make_unique<Wall>(Point(x, 0)));
        room3->addElement(std::make_unique<Wall>(Point(x, SCREEN_HEIGHT - 1)));
    }
    for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
        room3->addElement(std::make_unique<Wall>(Point(0, y)));
        room3->addElement(std::make_unique<Wall>(Point(SCREEN_WIDTH - 1, y)));
    }
    
    // Obstacles
    room3->addElement(std::make_unique<Obstacle>(Point(20, 10)));
    room3->addElement(std::make_unique<Obstacle>(Point(30, 10)));
    room3->addElement(std::make_unique<Obstacle>(Point(40, 10)));
    
    // Items
    room3->addElement(std::make_unique<Key>(Point(50, 15)));
    room3->addElement(std::make_unique<Door>(Point(70, 12), 3, 4));
    
    rooms.push_back(std::move(room3));
    
    // Room 4 - Final
    auto room4 = std::make_unique<Room>(4, true);
    
    // Borders only
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        room4->addElement(std::make_unique<Wall>(Point(x, 0)));
        room4->addElement(std::make_unique<Wall>(Point(x, SCREEN_HEIGHT - 1)));
    }
    for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
        room4->addElement(std::make_unique<Wall>(Point(0, y)));
        room4->addElement(std::make_unique<Wall>(Point(SCREEN_WIDTH - 1, y)));
    }
    
    rooms.push_back(std::move(room4));
}

void Game::showMenu() {
    clearScreen();
    gotoxy(30, 8);
    std::cout << "TEXT ADVENTURE GAME";
    gotoxy(30, 11);
    std::cout << "(1) Start New Game";
    gotoxy(30, 12);
    std::cout << "(8) Instructions";
    gotoxy(30, 13);
    std::cout << "(9) Exit";
    
    while (true) {
        if (_kbhit()) {
            char choice = _getch();
            if (choice == '1') {
                state = GameState::PLAYING;
                return;
            } else if (choice == '8') {
                showInstructions();
                showMenu();
                return;
            } else if (choice == '9') {
                state = GameState::EXIT;
                return;
            }
        }
        Sleep(50);
    }
}

void Game::showInstructions() {
    clearScreen();
    gotoxy(10, 3);
    std::cout << "=== INSTRUCTIONS ===";
    gotoxy(5, 5);
    std::cout << "Player 1: W(up) A(left) S(stay) D(right) X(down) E(dispose)";
    gotoxy(5, 6);
    std::cout << "Player 2: I(up) J(left) K(stay) L(right) M(down) O(dispose)";
    gotoxy(5, 8);
    std::cout << "ESC - Pause game";
    gotoxy(5, 10);
    std::cout << "Collect keys (K) to open doors (1,2,3)";
    gotoxy(5, 11);
    std::cout << "Torches (!) light the way";
    gotoxy(5, 12);
    std::cout << "Bombs (@) destroy walls when disposed";
    gotoxy(5, 13);
    std::cout << "Obstacles (*) can be pushed";
    gotoxy(5, 16);
    std::cout << "Press any key to continue...";
    _getch();
}

void Game::pauseGame() {
    clearScreen();  // FIXED: Clear screen when pausing
    gotoxy(20, 10);
    std::cout << "PAUSED - ESC to continue, H for menu";
    
    while (true) {
        if (_kbhit()) {
            char key = toUpperCase(_getch());
            if (key == Keys::ESC) {
                clearScreen();  // FIXED: Clear screen when resuming
                return;
            } else if (key == Keys::HOME) {
                state = GameState::MENU;
                return;
            }
        }
        Sleep(50);
    }
}

// UNIFIED: Handles input for a single player based on their keys
void Game::handlePlayerInput(Player* player, char key) {
    // Determine which player this is
    bool isPlayer1 = (player == player1.get());
    
    // Check direction keys
    if ((isPlayer1 && key == Keys::P1_UP) || (!isPlayer1 && key == Keys::P2_UP)) {
        player->setDirection(Direction::UP);
    } else if ((isPlayer1 && key == Keys::P1_DOWN) || (!isPlayer1 && key == Keys::P2_DOWN)) {
        player->setDirection(Direction::DOWN);
    } else if ((isPlayer1 && key == Keys::P1_LEFT) || (!isPlayer1 && key == Keys::P2_LEFT)) {
        player->setDirection(Direction::LEFT);
    } else if ((isPlayer1 && key == Keys::P1_RIGHT) || (!isPlayer1 && key == Keys::P2_RIGHT)) {
        player->setDirection(Direction::RIGHT);
    } else if ((isPlayer1 && key == Keys::P1_STAY) || (!isPlayer1 && key == Keys::P2_STAY)) {
        player->stop();
    } else if ((isPlayer1 && key == Keys::P1_DISPOSE) || (!isPlayer1 && key == Keys::P2_DISPOSE)) {
        if (player->hasItem()) {
            GameElement* item = player->disposeItem();
            item->setPosition(player->getPosition());
            
            // Activate bomb if disposing a bomb
            if (Bomb* bomb = dynamic_cast<Bomb*>(item)) {
                bomb->activate();
            }
        }
    }
}

// UNIFIED: Updates a single player's position
void Game::updatePlayer(Player* player, Player* otherPlayer) {
    if (player->getDirection() == Direction::NONE) return;
    
    Point nextPos = player->getNextPosition();
    Room* room = getCurrentRoom();
    
    // Check if position has obstacle
    Obstacle* obs = room->getObstacleAt(nextPos);
    if (obs) {
        // Try to push it
        if (room->tryPushObstacle(obs, player->getDirection())) {
            // Obstacle moved, player can move
            player->setPosition(nextPos);
        } else {
            // Can't push, stop player
            player->stop();
        }
        return;
    }
    
    // Check if other player is there
    if (nextPos == otherPlayer->getPosition()) {
        player->stop();
        return;
    }
    
    // Check if walkable
    if (room->isPositionWalkable(nextPos)) {
        player->setPosition(nextPos);
    } else {
        player->stop();
    }
}

void Game::checkCollisions() {
    Room* room = getCurrentRoom();
    
    // Check player 1
    GameElement* elem1 = room->getElementAt(player1->getPosition());
    if (elem1 && elem1->isCollectible() && !player1->hasItem()) {
        player1->pickUpItem(elem1);
        room->markElementAsCollected(elem1);
    }
    
    // Check player 2
    GameElement* elem2 = room->getElementAt(player2->getPosition());
    if (elem2 && elem2->isCollectible() && !player2->hasItem()) {
        player2->pickUpItem(elem2);
        room->markElementAsCollected(elem2);
    }
}

void Game::checkDoors() {
    Room* room = getCurrentRoom();
    
    // Check player 1
    Door* door1 = room->getDoorAt(player1->getPosition());
    if (door1 && player1->hasItem()) {
        if (dynamic_cast<Key*>(player1->getHeldItem())) {
            player1->disposeItem();  // Use key
            currentRoomIndex++;
            if (currentRoomIndex >= (int)rooms.size()) {
                currentRoomIndex = rooms.size() - 1;
            }
            if (getCurrentRoom()->getIsFinalRoom()) {
                player1ReachedEnd = true;
            }
            player1->setPosition(Point(5, 10));
            player1->stop();
            return;  // Room changed, stop checking
        }
    }
    
    // Check player 2
    Door* door2 = room->getDoorAt(player2->getPosition());
    if (door2 && player2->hasItem()) {
        if (dynamic_cast<Key*>(player2->getHeldItem())) {
            player2->disposeItem();  // Use key
            currentRoomIndex++;
            if (currentRoomIndex >= (int)rooms.size()) {
                currentRoomIndex = rooms.size() - 1;
            }
            if (getCurrentRoom()->getIsFinalRoom()) {
                player2ReachedEnd = true;
            }
            player2->setPosition(Point(5, 12));
            player2->stop();
        }
    }
}

void Game::drawGame() {
    clearScreen();
    getCurrentRoom()->draw();
    player1->draw();
    player2->draw();
    getCurrentRoom()->drawLegend(player1.get(), player2.get(), 2, 1);
}

void Game::startNewGame() {
    // Reset state
    currentRoomIndex = 0;
    player1ReachedEnd = false;
    player2ReachedEnd = false;
    
    // Reset players
    player1 = std::make_unique<Player>(Point(5, 10), Chars::PLAYER1);
    player2 = std::make_unique<Player>(Point(5, 12), Chars::PLAYER2);
    
    // Recreate rooms
    rooms.clear();
    createRooms();
    
    hideCursor();
    clearScreen();
    
    // Game loop
    while (state == GameState::PLAYING && !(player1ReachedEnd && player2ReachedEnd)) {
        // Input
        if (_kbhit()) {
            char key = toUpperCase(_getch());
            
            if (key == Keys::ESC) {
                pauseGame();
                if (state != GameState::PLAYING) break;
                continue;
            }
            
            // Try both players
            handlePlayerInput(player1.get(), key);
            handlePlayerInput(player2.get(), key);
        }
        
        // Update
        updatePlayer(player1.get(), player2.get());
        updatePlayer(player2.get(), player1.get());
        checkCollisions();
        checkDoors();
        getCurrentRoom()->updateBombs();
        
        // Draw
        drawGame();
        
        Sleep(GAME_CYCLE_DELAY);
    }
    
    // Victory
    if (player1ReachedEnd && player2ReachedEnd) {
        clearScreen();
        gotoxy(30, 12);
        std::cout << "CONGRATULATIONS! YOU WON!";
        Sleep(3000);
    }
    
    showCursor();
}

void Game::run() {
    while (state != GameState::EXIT) {
        showMenu();
        if (state == GameState::PLAYING) {
            startNewGame();
            state = GameState::MENU;
        }
    }
}

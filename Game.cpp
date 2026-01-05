#include "Game.h"
#include "GameConfig.h"
#include "Wall.h"
#include "Key.h"
#include "Door.h"
#include "Torch.h"
#include "Bomb.h"
#include "Obstacle.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

Game::Game() 
    : currentRoomIndex(0), player1ReachedEnd(false), player2ReachedEnd(false), state(GameState::MENU) {
    player1 = std::make_unique<Player>(Point(5, 10), Chars::PLAYER1);
    player2 = std::make_unique<Player>(Point(5, 12), Chars::PLAYER2);
    loadRoomsFromFiles();
}

void Game::loadRoomsFromFiles() {
    // Find all screen files in lexicographical order
    std::vector<std::string> screenFiles;
    
    // Look for adv-world*.screen files in the current directory
    for (int i = 1; i <= 99; i++) {
        std::ostringstream filename;
        filename << "adv-world_" << (i < 10 ? "0" : "") << i << ".screen";
        
        std::ifstream file(filename.str());
        if (file.good()) {
            screenFiles.push_back(filename.str());
        }
    }
    
    // If no screen files found, show error and exit
    if (screenFiles.empty()) {
        std::cerr << "Error: No screen files found (adv-world*.screen)" << std::endl;
        return;
    }
    
    // Load each screen file
    int roomId = 1;
    for (const auto& filename : screenFiles) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filename << std::endl;
            continue;
        }
        
        // Determine if this is the final room (last file in the list)
        bool isFinalRoom = (roomId == (int)screenFiles.size());
        auto room = std::make_unique<Room>(roomId, isFinalRoom);
        
        Point legendPos(2, 1);  // Default legend position
        bool legendFound = false;
        
        // Read the file line by line (28 lines: 3 for legend area + 25 for game area)
        std::string line;
        int y = 0;
        const int TOTAL_LINES = SCREEN_OFFSET_Y + SCREEN_HEIGHT;  // 3 + 25 = 28
        while (std::getline(file, line) && y < TOTAL_LINES) {
            // Pad or truncate line to SCREEN_WIDTH
            if (line.length() < SCREEN_WIDTH) {
                line.resize(SCREEN_WIDTH, ' ');
            }
            
            // Parse each character in the line
            for (int x = 0; x < SCREEN_WIDTH && x < (int)line.length(); x++) {
                char ch = line[x];
                Point pos(x, y);
                
                // For game elements in the game area (y >= SCREEN_OFFSET_Y), 
                // adjust position to game coordinates (0-24)
                Point gamePos(x, y >= SCREEN_OFFSET_Y ? y - SCREEN_OFFSET_Y : y);
                
                switch (ch) {
                    case 'W':  // Wall
                        if (y >= SCREEN_OFFSET_Y) {
                            room->addElement(std::make_unique<Wall>(gamePos));
                        }
                        break;
                    
                    case 'K':  // Key
                        if (y >= SCREEN_OFFSET_Y) {
                            room->addElement(std::make_unique<Key>(gamePos));
                        }
                        break;
                    
                    case '!':  // Torch
                        if (y >= SCREEN_OFFSET_Y) {
                            room->addElement(std::make_unique<Torch>(gamePos));
                        }
                        break;
                    
                    case '@':  // Bomb
                        if (y >= SCREEN_OFFSET_Y) {
                            room->addElement(std::make_unique<Bomb>(gamePos));
                        }
                        break;
                    
                    case '*':  // Obstacle
                        if (y >= SCREEN_OFFSET_Y) {
                            room->addElement(std::make_unique<Obstacle>(gamePos));
                        }
                        break;
                    
                    case 'L':  // Legend position marker (keep file coordinates)
                        legendPos = pos;
                        legendFound = true;
                        break;
                    
                    case ' ':  // Empty space
                    case '.':  // Alternative empty space marker
                        // Walkable, no element needed
                        break;
                    
                    default:
                        // Check if it's a door number (1-9)
                        if (ch >= '1' && ch <= '9') {
                            if (y >= SCREEN_OFFSET_Y) {
                                int targetRoom = ch - '0';
                                room->addElement(std::make_unique<Door>(gamePos, roomId, targetRoom));
                            }
                        }
                        // Otherwise ignore unknown characters
                        break;
                }
            }
            y++;
        }
        
        file.close();
        
        // Store legend position for this room
        legendPositions.push_back(legendPos);
        
        // Add room to the game
        rooms.push_back(std::move(room));
        roomId++;
    }
    
    // Add a final empty room if we don't have at least one room
    if (rooms.empty()) {
        auto finalRoom = std::make_unique<Room>(1, true);
        
        // Add borders only
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            finalRoom->addElement(std::make_unique<Wall>(Point(x, 0)));
            finalRoom->addElement(std::make_unique<Wall>(Point(x, SCREEN_HEIGHT - 1)));
        }
        for (int y = 1; y < SCREEN_HEIGHT - 1; y++) {
            finalRoom->addElement(std::make_unique<Wall>(Point(0, y)));
            finalRoom->addElement(std::make_unique<Wall>(Point(SCREEN_WIDTH - 1, y)));
        }
        
        rooms.push_back(std::move(finalRoom));
        legendPositions.push_back(Point(2, 1));
    }
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
    
    // Get legend position for current room
    Point legendPos = legendPositions[currentRoomIndex];
    getCurrentRoom()->drawLegend(player1.get(), player2.get(), legendPos.getX(), legendPos.getY());
}

void Game::startNewGame() {
    // Reset state
    currentRoomIndex = 0;
    player1ReachedEnd = false;
    player2ReachedEnd = false;
    
    // Reset players
    player1 = std::make_unique<Player>(Point(5, 10), Chars::PLAYER1);
    player2 = std::make_unique<Player>(Point(5, 12), Chars::PLAYER2);
    
    // Reload rooms from files
    rooms.clear();
    legendPositions.clear();
    loadRoomsFromFiles();
    
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

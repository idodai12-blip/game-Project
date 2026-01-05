#include "Game.h"
#include "GameConfig.h"
#include "Wall.h"
#include "Key.h"
#include "Door.h"
#include "Torch.h"
#include "Bomb.h"
#include "Obstacle.h"
#include "Riddle.h"
#include "Switch.h"
#include "Spring.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

Game::Game() 
    : currentRoomIndex(0), player1ReachedEnd(false), player2ReachedEnd(false), 
      state(GameState::MENU), activeRiddle(nullptr), riddlePlayer(nullptr) {
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
        
        // Store all lines to detect springs (multi-char elements)
        std::vector<std::string> allLines;
        std::string line;
        const int TOTAL_LINES = SCREEN_OFFSET_Y + SCREEN_HEIGHT;  // 3 + 25 = 28
        while (std::getline(file, line) && (int)allLines.size() < TOTAL_LINES) {
            // Pad or truncate line to SCREEN_WIDTH
            if (line.length() < SCREEN_WIDTH) {
                line.resize(SCREEN_WIDTH, ' ');
            }
            allLines.push_back(line);
        }
        file.close();
        
        // Track which positions are part of springs to avoid double-processing
        std::vector<std::vector<bool>> isSpring(TOTAL_LINES, std::vector<bool>(SCREEN_WIDTH, false));
        
        // First pass: detect springs (horizontal and vertical sequences of '#')
        for (int y = SCREEN_OFFSET_Y; y < (int)allLines.size(); y++) {
            for (int x = 0; x < SCREEN_WIDTH && x < (int)allLines[y].length(); x++) {
                if (allLines[y][x] == '#' && !isSpring[y][x]) {
                    Point gamePos(x, y - SCREEN_OFFSET_Y);
                    
                    // Check for horizontal spring (RIGHT direction)
                    int hLen = 0;
                    while (x + hLen < SCREEN_WIDTH && allLines[y][x + hLen] == '#') {
                        hLen++;
                    }
                    
                    // Check for vertical spring (DOWN direction)
                    int vLen = 0;
                    while (y + vLen < (int)allLines.size() && allLines[y + vLen][x] == '#') {
                        vLen++;
                    }
                    
                    // Choose the longer sequence
                    if (hLen > vLen) {
                        // Horizontal spring - determine direction based on walls
                        // Check if there's a wall on the right (spring faces RIGHT)
                        bool wallOnRight = (x + hLen < SCREEN_WIDTH && allLines[y][x + hLen] == 'W');
                        Direction springDir = wallOnRight ? Direction::RIGHT : Direction::LEFT;
                        
                        room->addElement(std::make_unique<Spring>(gamePos, springDir, hLen));
                        
                        // Mark all positions as part of spring
                        for (int i = 0; i < hLen; i++) {
                            isSpring[y][x + i] = true;
                        }
                    } else if (vLen > 0) {
                        // Vertical spring - determine direction based on walls
                        // Check if there's a wall below (spring faces DOWN)
                        bool wallBelow = (y + vLen < (int)allLines.size() && allLines[y + vLen][x] == 'W');
                        Direction springDir = wallBelow ? Direction::DOWN : Direction::UP;
                        
                        room->addElement(std::make_unique<Spring>(gamePos, springDir, vLen));
                        
                        // Mark all positions as part of spring
                        for (int i = 0; i < vLen; i++) {
                            isSpring[y + i][x] = true;
                        }
                    }
                }
            }
        }
        
        // Second pass: parse regular elements
        for (int y = 0; y < (int)allLines.size(); y++) {
            for (int x = 0; x < SCREEN_WIDTH && x < (int)allLines[y].length(); x++) {
                char ch = allLines[y][x];
                Point pos(x, y);
                Point gamePos(x, y >= SCREEN_OFFSET_Y ? y - SCREEN_OFFSET_Y : y);
                
                // Skip spring positions
                if (y >= SCREEN_OFFSET_Y && isSpring[y][x]) {
                    continue;
                }
                
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
        }
        
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
    gotoxy(5, 14);
    std::cout << "Riddles (?) - Press 4 to solve and pass through";
    gotoxy(5, 15);
    std::cout << "Switches (\\//) - Step on them to toggle, open doors when all ON";
    gotoxy(5, 16);
    std::cout << "Springs (#) - Launch you in their direction when compressed";
    gotoxy(5, 18);
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
    Room* room = getCurrentRoom();
    Direction moveDir = player->getDirection();
    
    // If under spring effect, use spring direction and velocity
    if (player->isUnderSpringEffect()) {
        Direction springDir = player->getSpringDirection();
        int velocity = player->getSpringVelocity();
        
        // Check if player is trying to move backward against spring
        Point backwardCheck = directionToPoint(springDir);
        backwardCheck = Point(-backwardCheck.getX(), -backwardCheck.getY());
        Point tryMove = directionToPoint(moveDir);
        
        // Ignore backward or stay commands
        if (moveDir == Direction::NONE || 
            (tryMove.getX() == backwardCheck.getX() && tryMove.getY() == backwardCheck.getY())) {
            // Just use spring direction
            moveDir = springDir;
        } else {
            // Allow lateral movement (perpendicular to spring)
            // Spring moves in springDir, player can add lateral component
            // For now, simplified: move in spring direction with lateral offset
            Point springMove = directionToPoint(springDir);
            Point lateralMove = directionToPoint(moveDir);
            
            // Move multiple times based on velocity
            for (int v = 0; v < velocity; v++) {
                Point nextPos = player->getPosition() + springMove;
                
                // Check if other player is there
                if (nextPos == otherPlayer->getPosition()) {
                    // Transfer spring effect to other player
                    otherPlayer->setSpringEffect(springDir, velocity, velocity * velocity);
                    otherPlayer->stop();
                    player->stop();
                    return;
                }
                
                // Check walkability
                if (room->isPositionWalkable(nextPos) && room->getElementAt(nextPos) == nullptr) {
                    player->setPosition(nextPos);
                } else {
                    // Hit obstacle, stop
                    player->stop();
                    player->clearSpringEffect();
                    return;
                }
            }
            
            // Apply lateral movement (once)
            if (moveDir != springDir && moveDir != Direction::NONE) {
                Point nextLateral = player->getPosition() + lateralMove;
                if (room->isPositionWalkable(nextLateral) && 
                    room->getElementAt(nextLateral) == nullptr &&
                    nextLateral != otherPlayer->getPosition()) {
                    player->setPosition(nextLateral);
                }
            }
            
            return;
        }
        
        // Move according to spring velocity
        for (int v = 0; v < velocity; v++) {
            Point nextPos = player->getPosition() + directionToPoint(springDir);
            
            // Check if other player is there
            if (nextPos == otherPlayer->getPosition()) {
                // Transfer spring effect to other player
                otherPlayer->setSpringEffect(springDir, velocity, velocity * velocity);
                otherPlayer->stop();
                player->stop();
                return;
            }
            
            // Check walkability
            if (room->isPositionWalkable(nextPos) && room->getElementAt(nextPos) == nullptr) {
                player->setPosition(nextPos);
            } else {
                // Hit obstacle, stop
                player->stop();
                player->clearSpringEffect();
                return;
            }
        }
        
        return;
    }
    
    // Normal movement (no spring effect)
    if (moveDir == Direction::NONE) return;
    
    Point nextPos = player->getNextPosition();
    
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
            // Check if switches are activated for this door
            if (!room->areSwitchesActivated(door1->getSwitchGroupId())) {
                return;  // Door is locked by switches
            }
            
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
            // Check if switches are activated for this door
            if (!room->areSwitchesActivated(door2->getSwitchGroupId())) {
                return;  // Door is locked by switches
            }
            
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

void Game::checkSwitches() {
    Room* room = getCurrentRoom();
    
    // Check player 1 position (only toggle when stepping onto a switch)
    // We check if player moved this frame by checking direction
    if (player1->getDirection() != Direction::NONE) {
        Switch* sw1 = room->getSwitchAt(player1->getPosition());
        if (sw1) {
            sw1->toggle();
        }
    }
    
    // Check player 2
    if (player2->getDirection() != Direction::NONE) {
        Switch* sw2 = room->getSwitchAt(player2->getPosition());
        if (sw2) {
            sw2->toggle();
        }
    }
}

void Game::checkSprings() {
    Room* room = getCurrentRoom();
    
    // Helper to get opposite direction
    auto getOppositeDir = [](Direction dir) -> Direction {
        switch (dir) {
            case Direction::UP: return Direction::DOWN;
            case Direction::DOWN: return Direction::UP;
            case Direction::LEFT: return Direction::RIGHT;
            case Direction::RIGHT: return Direction::LEFT;
            default: return Direction::NONE;
        }
    };
    
    // Check player 1
    Spring* spring1 = room->getSpringAt(player1->getPosition());
    if (spring1 && !player1->isUnderSpringEffect()) {
        Direction springDir = spring1->getAlignment();
        Direction playerDir = player1->getDirection();
        Direction launchDir = getOppositeDir(springDir);
        
        // Get player's position index in the spring (0-based)
        int posIndex = spring1->getPositionIndex(player1->getPosition());
        
        // Calculate compression based on spring alignment
        // For RIGHT-facing springs: player compresses from left, compression increases with index
        // For LEFT-facing springs: player compresses from right, compression decreases with index
        int compressed;
        if (springDir == Direction::RIGHT || springDir == Direction::DOWN) {
            compressed = posIndex + 1;
        } else { // LEFT or UP
            compressed = spring1->getLength() - posIndex;
        }
        
        // Update spring visual compression
        spring1->compress(compressed);
        
        // Determine if spring should release
        bool shouldRelease = false;
        
        // Case 1: Player is moving in the spring direction
        if (playerDir == springDir) {
            // Check if next position hits a wall or is beyond the spring
            Point springMove = directionToPoint(springDir);
            Point nextPos = player1->getPosition() + springMove;
            
            // If next position is a wall or beyond spring, release
            if (room->isWall(nextPos) || !spring1->isPartOfSpring(nextPos)) {
                shouldRelease = true;
            }
        }
        // Case 2: Player stops on spring (STAY command)
        else if (playerDir == Direction::NONE) {
            shouldRelease = true;
        }
        // Case 3: Player is on spring but moving in different direction
        else {
            shouldRelease = true;
        }
        
        // Release spring and launch player
        if (shouldRelease) {
            int velocity = compressed;
            int cycles = velocity * velocity;
            player1->setSpringEffect(launchDir, velocity, cycles);
            // Collapse all springs when launched
            for (Spring* s : room->getSprings()) {
                if (s) s->release();
            }
        }
    }
    
    // Check player 2
    Spring* spring2 = room->getSpringAt(player2->getPosition());
    if (spring2 && !player2->isUnderSpringEffect()) {
        Direction springDir = spring2->getAlignment();
        Direction playerDir = player2->getDirection();
        Direction launchDir = getOppositeDir(springDir);
        
        // Get player's position index in the spring (0-based)
        int posIndex = spring2->getPositionIndex(player2->getPosition());
        
        // Calculate compression based on spring alignment
        // For RIGHT-facing springs: player compresses from left, compression increases with index
        // For LEFT-facing springs: player compresses from right, compression decreases with index
        int compressed;
        if (springDir == Direction::RIGHT || springDir == Direction::DOWN) {
            compressed = posIndex + 1;
        } else { // LEFT or UP
            compressed = spring2->getLength() - posIndex;
        }
        
        // Update spring visual compression
        spring2->compress(compressed);
        
        // Determine if spring should release
        bool shouldRelease = false;
        
        // Case 1: Player is moving in the spring direction
        if (playerDir == springDir) {
            // Check if next position hits a wall or is beyond the spring
            Point springMove = directionToPoint(springDir);
            Point nextPos = player2->getPosition() + springMove;
            
            // If next position is a wall or beyond spring, release
            if (room->isWall(nextPos) || !spring2->isPartOfSpring(nextPos)) {
                shouldRelease = true;
            }
        }
        // Case 2: Player stops on spring (STAY command)
        else if (playerDir == Direction::NONE) {
            shouldRelease = true;
        }
        // Case 3: Player is on spring but moving in different direction
        else {
            shouldRelease = true;
        }
        
        // Release spring and launch player
        if (shouldRelease) {
            int velocity = compressed;
            int cycles = velocity * velocity;
            player2->setSpringEffect(launchDir, velocity, cycles);
            // Collapse all springs when launched
            for (Spring* s : room->getSprings()) {
                if (s) s->release();
            }
        }
    }
}

void Game::updateSpringEffects() {
    // Decrement spring cycles for both players
    if (player1->isUnderSpringEffect()) {
        player1->decrementSpringCycles();
        if (!player1->isUnderSpringEffect()) {
            player1->clearSpringEffect();
        }
    }
    
    if (player2->isUnderSpringEffect()) {
        player2->decrementSpringCycles();
        if (!player2->isUnderSpringEffect()) {
            player2->clearSpringEffect();
        }
    }
}

void Game::checkRiddles() {
    Room* room = getCurrentRoom();
    
    // Check player 1
    Riddle* riddle1 = room->getRiddleAt(player1->getPosition());
    if (riddle1 && !riddle1->isActive()) {
        riddle1->setActive(true);
        activeRiddle = riddle1;
        riddlePlayer = player1.get();
        player1->stop();  // Stop player movement
        return;
    }
    
    // Check player 2
    Riddle* riddle2 = room->getRiddleAt(player2->getPosition());
    if (riddle2 && !riddle2->isActive()) {
        riddle2->setActive(true);
        activeRiddle = riddle2;
        riddlePlayer = player2.get();
        player2->stop();  // Stop player movement
    }
}

void Game::drawRiddleOverlay() {
    clearScreen();
    gotoxy(30, 10);
    std::cout << "Hello World";
    gotoxy(30, 12);
    std::cout << "Press 4 to solve the riddle";
}

void Game::drawGame() {
    if (activeRiddle) {
        drawRiddleOverlay();
        return;
    }
    
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
    activeRiddle = nullptr;
    riddlePlayer = nullptr;
    
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
            
            // Handle riddle solving
            if (activeRiddle && key == Keys::SOLVE_RIDDLE) {
                // Move player to riddle position and remove riddle
                if (riddlePlayer) {
                    riddlePlayer->setPosition(activeRiddle->getPosition());
                }
                getCurrentRoom()->markElementAsCollected(activeRiddle);
                activeRiddle->setActive(false);
                activeRiddle = nullptr;
                riddlePlayer = nullptr;
                continue;
            }
            
            // Don't process movement if riddle is active
            if (!activeRiddle) {
                // Try both players
                handlePlayerInput(player1.get(), key);
                handlePlayerInput(player2.get(), key);
            }
        }
        
        // Update (skip if riddle is active)
        if (!activeRiddle) {
            updatePlayer(player1.get(), player2.get());
            updatePlayer(player2.get(), player1.get());
            checkSwitches();
            checkCollisions();
            checkDoors();
            checkSprings();
            checkRiddles();
            getCurrentRoom()->updateBombs();
            updateSpringEffects();
        }
        
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

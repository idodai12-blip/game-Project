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

Game::Game() 
    : currentRoomIndex(0), player1ReachedEnd(false), player2ReachedEnd(false), 
      state(GameState::MENU), activeRiddle(nullptr), riddlePlayer(nullptr) {
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
    room1->addElement(std::make_unique<Riddle>(Point(40, 10)));
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
    
    // Add switches (group 1) and switch-controlled door
    room2->addElement(std::make_unique<Switch>(Point(15, 8), 1));
    room2->addElement(std::make_unique<Switch>(Point(15, 16), 1));
    room2->addElement(std::make_unique<Door>(Point(70, 12), 2, 3, 1));  // Door requires switch group 1
    
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
    
    // Add a horizontal spring (3 chars long, facing right)
    room3->addElement(std::make_unique<Spring>(Point(10, 15), Direction::RIGHT, 3));
    
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
                    otherPlayer->setSpringEffect(springDir, velocity, player->getSpringVelocity() * player->getSpringVelocity());
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
    
    // Check player 1
    Spring* spring1 = room->getSpringAt(player1->getPosition());
    if (spring1 && !player1->isUnderSpringEffect()) {
        // Player stepped onto a spring
        Direction springDir = spring1->getAlignment();
        Direction playerDir = player1->getDirection();
        
        // Check if player is moving in the spring direction
        if (playerDir == springDir) {
            // Start compressing the spring
            // Calculate how many spring chars the player will compress
            Point springMove = directionToPoint(springDir);
            int compressed = 0;
            Point checkPos = player1->getPosition();
            
            // Count consecutive spring positions ahead
            for (int i = 0; i < spring1->getLength(); i++) {
                checkPos = checkPos + springMove;
                if (spring1->isPartOfSpring(checkPos)) {
                    compressed++;
                } else {
                    break;
                }
                
                // Stop if hit a wall
                if (room->isWall(checkPos)) {
                    break;
                }
            }
            
            // Check if next position after spring compression is a wall or stay command
            Point nextAfterSpring = player1->getPosition() + Point(springMove.getX() * (compressed + 1), 
                                                                     springMove.getY() * (compressed + 1));
            bool hitWall = room->isWall(nextAfterSpring);
            
            if (compressed > 0 && (hitWall || playerDir == Direction::NONE)) {
                // Launch the player
                int velocity = compressed;
                int cycles = compressed * compressed;
                player1->setSpringEffect(springDir, velocity, cycles);
                spring1->release();
            }
        }
    }
    
    // Check player 2
    Spring* spring2 = room->getSpringAt(player2->getPosition());
    if (spring2 && !player2->isUnderSpringEffect()) {
        // Player stepped onto a spring
        Direction springDir = spring2->getAlignment();
        Direction playerDir = player2->getDirection();
        
        // Check if player is moving in the spring direction
        if (playerDir == springDir) {
            // Start compressing the spring
            // Calculate how many spring chars the player will compress
            Point springMove = directionToPoint(springDir);
            int compressed = 0;
            Point checkPos = player2->getPosition();
            
            // Count consecutive spring positions ahead
            for (int i = 0; i < spring2->getLength(); i++) {
                checkPos = checkPos + springMove;
                if (spring2->isPartOfSpring(checkPos)) {
                    compressed++;
                } else {
                    break;
                }
                
                // Stop if hit a wall
                if (room->isWall(checkPos)) {
                    break;
                }
            }
            
            // Check if next position after spring compression is a wall or stay command
            Point nextAfterSpring = player2->getPosition() + Point(springMove.getX() * (compressed + 1), 
                                                                     springMove.getY() * (compressed + 1));
            bool hitWall = room->isWall(nextAfterSpring);
            
            if (compressed > 0 && (hitWall || playerDir == Direction::NONE)) {
                // Launch the player
                int velocity = compressed;
                int cycles = compressed * compressed;
                player2->setSpringEffect(springDir, velocity, cycles);
                spring2->release();
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
    getCurrentRoom()->drawLegend(player1.get(), player2.get(), 2, 1);
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

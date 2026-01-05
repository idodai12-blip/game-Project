#pragma once
#include <windows.h>
#include <conio.h>
#include <iostream>

// Screen constants
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const int SCREEN_OFFSET_Y = 3;  // Game area starts 3 lines down
const int GAME_CYCLE_DELAY = 120;

// Player control keys
namespace Keys {
    // Player 1
    const char P1_UP = 'W';
    const char P1_DOWN = 'X';
    const char P1_LEFT = 'A';
    const char P1_RIGHT = 'D';
    const char P1_STAY = 'S';
    const char P1_DISPOSE = 'E';
    
    // Player 2
    const char P2_UP = 'I';
    const char P2_DOWN = 'M';
    const char P2_LEFT = 'J';
    const char P2_RIGHT = 'L';
    const char P2_STAY = 'K';
    const char P2_DISPOSE = 'O';
    
    // System
    const char ESC = 27;
    const char HOME = 'H';
}

// Display characters
namespace Chars {
    const char PLAYER1 = '$';
    const char PLAYER2 = '&';
    const char WALL = 'W';
    const char KEY = 'K';
    const char DOOR_BASE = '1'; // 1-9
    const char TORCH = '!';
    const char BOMB = '@';
    const char OBSTACLE = '*';
}

// Utility functions
void gotoxy(int x, int y);
void clearScreen();
void hideCursor();
void showCursor();
char toUpperCase(char c);

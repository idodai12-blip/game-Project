# Spring Test Plan

## Overview
This document describes the test cases for the spring functionality in the game.

## Spring Setup
Two horizontal springs have been added to `adv-world_01.screen` at line 21:
- Spring 1: Position (5, 17) - `###W` - 3 chars long, launches LEFT (opposite of RIGHT alignment toward wall)
- Spring 2: Position (54, 17) - `###W` - 3 chars long, launches LEFT (opposite of RIGHT alignment toward wall)

## Test Cases

### Test Case 1: Player on spring, moves until hitting wall, launches with speed 3 for 9 cycles
**Setup:**
1. Start the game (Player 1 starts at position 5, 10)
2. Move Player 1 to position (4, 17) - one position BEFORE Spring 1 which starts at (5, 17)

**Steps (4 gameplay loops):**
1. **Loop 1**: Player at (4, 17) - next to spring, press 'D' (move right)
2. **Loop 2**: Player moves to (5, 17) - spring position index 0, compression = 1
3. **Loop 3**: Player at (5, 17), press 'D', player moves to (6, 17) - spring position index 1, compression = 2
4. **Loop 4**: Player at (6, 17), press 'D', player moves to (7, 17) - spring position index 2, compression = 3
5. **Loop 5**: Player at (7, 17), press 'D', player tries to move to (8, 17) but hits wall

**Expected Result:**
- After loop 5, when player hits the wall, spring releases
- Player is launched LEFT (opposite to spring's RIGHT direction)
- Launch velocity = 3 (based on compression amount)
- Effect lasts for 9 cycles (3² = 9)
- Player moves 3 positions to the left per cycle for 9 cycles
- All springs in the room visually collapse (release)

### Test Case 2: Player on spring then stops, launches with speed 1 for 1 cycle
**Setup:**
1. Move Player 1 to position (5, 17) - first position of Spring 1 (index 0)

**Steps:**
1. Player is at (5, 17) - on spring position index 0, compression = 1
2. Press 'S' (STAY) to stop

**Expected Result:**
- Player is on the spring and stops
- Spring releases with compression = 1
- Player is launched LEFT with velocity = 1
- Effect lasts for 1 cycle (1² = 1)
- Player moves 1 position to the left for 1 cycle
- All springs in the room visually collapse

### Test Case 3: Player walks on spring and changes direction
**Setup:**
1. Move Player 1 to position (4, 17) - one position before Spring 1

**Steps:**
1. Press 'D' to start moving onto the spring
2. Player moves to (5, 17) - spring position index 0, compression = 1
3. Press 'D' again to continue
4. Player moves to (6, 17) - spring position index 1, compression = 2
5. Press 'W' (UP) or 'X' (DOWN) to change direction

**Expected Result:**
- Player is launched in the opposite direction of the spring (LEFT)
- Launch velocity = 2 (based on compression at position index 1)
- Effect lasts for 4 cycles (2² = 4)
- Player moves 2 positions to the left per cycle for 4 cycles
- All springs in the room visually collapse

### Test Case 4: All springs collapse when any spring launches player
**Setup:**
1. Two springs exist in the room (both at line 21)
2. Move Player 1 to Spring 1 at (5, 17)
3. Both springs should be visible initially

**Steps:**
1. Trigger Spring 1 by any method (hitting wall, stopping, or changing direction)

**Expected Result:**
- Both Spring 1 AND Spring 2 visually collapse (release)
- Both springs' display length returns to full length
- This happens regardless of which spring was triggered

## Implementation Details

### Spring Loading
- Springs are loaded from screen files during `Game::loadRoomsFromFiles()`
- Horizontal sequences of '#' characters are detected
- Spring direction is determined by checking for adjacent walls
- If wall is on the right (e.g., `###W`), spring direction is RIGHT, launches LEFT
- If wall is on the left (e.g., `W###`), spring direction is LEFT, launches RIGHT

### Spring Trigger Logic
- `Game::checkSprings()` is called each game cycle
- When player is on a spring and not already under spring effect:
  - **Compression calculation**: compression = position_index + 1
    - At position index 0 (first position): compression = 1
    - At position index 1 (second position): compression = 2
    - At position index 2 (third position): compression = 3
  - **Moving in spring direction**: Spring compresses, checks if next position hits wall or is beyond spring
  - **Stops (STAY command)**: Releases with current compression
  - **Changes direction**: Releases with current compression
- Launch is always in opposite direction to spring alignment
- Launch velocity = compression amount
- Launch cycles = velocity²
- All springs in room collapse when any spring releases

### Spring Compression Progression
For a 3-character spring (`###W`):
- Player walks from left to right (spring direction)
- Position index 0: Player at first '#', compression = 1
- Position index 1: Player at second '#', compression = 2
- Position index 2: Player at third '#', compression = 3
- Next move would hit wall 'W', spring releases with compression = 3

### Spring Visual Behavior
- Springs draw with compression: `displayLength = length - compressedLength`
- When compressed, fewer '#' characters are shown
- When released (`spring->release()`), compression resets to 0

## Controls
- **Player 1**: W(up) A(left) S(stay) D(right) X(down)
- **Player 2**: I(up) J(left) K(stay) L(right) M(down)

## Notes
- This is a Windows application using Visual Studio
- Compile using Visual Studio with the .sln file
- The game uses Windows console API for display and input
- Springs only affect players not already under a spring effect
- Spring effect velocity determines how many positions to move per cycle
- Spring effect continues until cycles remaining reaches 0

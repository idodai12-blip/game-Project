Student ID: 206439135

Implemented features:
- 2 players ($, &)
- Walls (W)
- Keys (K) - collectible
- Doors (1-9) - require keys to pass
- Torches (!) - collectible
- Bombs (@) - collectible, activate on dispose, explode after 5 cycles
- Obstacles (*) - can be pushed by players

All major bugs fixed based on grader feedback:
- Fixed runtime error in Room::removeElement (now using markElementAsCollected)
- Fixed torch and key collection
- Fixed advance to second room
- Implemented obstacle pushing
- Fixed pause screen clearing
- Moved bomb logic to Room class methods
- Avoided raw pointers from unique_ptr::get() where possible
- Unified player logic (reduced code duplication)
- Added copy constructor and assignment operator blocking
- Separated each element class to its own header file

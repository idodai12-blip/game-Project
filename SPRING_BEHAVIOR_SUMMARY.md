# Spring Behavior Summary

## Quick Reference

### Spring Configuration
- Location: `adv-world_01.screen`, line 21 (game Y coordinate 17)
- Spring 1: X positions 5, 6, 7 (followed by wall at X=8)
- Spring 2: X positions 54, 55, 56 (followed by wall at X=57)
- Visual: `###W` (3 spring characters, wall on right)
- Alignment: RIGHT (spring faces toward the wall)
- Launch direction: LEFT (opposite of alignment)

## Test Case 1: Full Compression (Speed 3 for 9 Cycles)

**Player Position Progression:**
```
Loop 1: X=4, Y=17 (before spring) -> press D -> move to X=5
Loop 2: X=5, Y=17 (spring index 0, compression=1) -> press D -> move to X=6
Loop 3: X=6, Y=17 (spring index 1, compression=2) -> press D -> move to X=7
Loop 4: X=7, Y=17 (spring index 2, compression=3) -> press D -> try X=8 (WALL!)
Loop 5+: Spring releases, launches LEFT with velocity=3 for 9 cycles
```

**Result:**
- Velocity: 3 positions per cycle
- Duration: 9 cycles (3²)
- Direction: LEFT (X decreases by 3 each cycle)
- All springs collapse visually

## Test Case 2: Stop on Spring (Speed 1 for 1 Cycle)

**Player Position:**
```
Player at X=5, Y=17 (spring index 0, compression=1) -> press S (STAY)
Spring releases, launches LEFT with velocity=1 for 1 cycle
```

**Result:**
- Velocity: 1 position per cycle
- Duration: 1 cycle (1²)
- Direction: LEFT
- All springs collapse visually

## Test Case 3: Direction Change (Speed Based on Position)

**Player Position Progression:**
```
Player at X=4, Y=17 (before spring) -> press D -> move to X=5
Player at X=5, Y=17 (spring index 0, compression=1) -> press D -> move to X=6
Player at X=6, Y=17 (spring index 1, compression=2) -> press W or X (change direction)
Spring releases, launches LEFT with velocity=2 for 4 cycles
```

**Result:**
- Velocity: 2 positions per cycle (based on compression at index 1)
- Duration: 4 cycles (2²)
- Direction: LEFT
- All springs collapse visually

## Key Implementation Points

1. **Compression Calculation**: `compression = position_index + 1`
   - Index 0 → compression 1
   - Index 1 → compression 2
   - Index 2 → compression 3

2. **Release Triggers**:
   - Moving in spring direction AND (next position is wall OR not part of spring)
   - Player stops (STAY command)
   - Player changes direction

3. **Launch Formula**:
   - Velocity = compression amount
   - Cycles = velocity²
   - Direction = opposite of spring alignment

4. **All Springs Collapse**: When any spring releases, ALL springs in the room visually collapse

## Visual Compression Display

Spring: `###W` (length 3)
- No compression: `###W` (displays 3 characters)
- Compression 1: `##W` (displays 2 characters)
- Compression 2: `#W` (displays 1 character)
- Compression 3: `W` (displays 0 characters, fully compressed)
- Released: `###W` (displays 3 characters again)

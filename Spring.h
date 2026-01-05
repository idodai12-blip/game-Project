#pragma once
#include "GameElement.h"
#include "Direction.h"
#include "GameConfig.h"
#include <vector>
#include <iostream>

class Spring : public GameElement {
private:
    Direction alignment;  // Direction the spring faces (direction it will launch)
    int length;          // Original length of spring (number of chars)
    int compressedLength;  // Currently compressed chars
    bool isCompressed;
    
public:
    Spring(Point pos, Direction dir, int springLength) 
        : GameElement(pos, '#'), alignment(dir), length(springLength), 
          compressedLength(0), isCompressed(false) {}
    
    bool canPlayerPass() const override { return true; }
    
    // Override draw to show all spring positions
    void draw() const override {
        Point springDir = directionToPoint(alignment);
        int displayLength = isCompressed ? (length - compressedLength) : length;
        
        for (int i = 0; i < displayLength; i++) {
            Point drawPos = position + Point(springDir.getX() * i, springDir.getY() * i);
            gotoxy(drawPos.getX(), drawPos.getY() + SCREEN_OFFSET_Y);
            std::cout << displayChar;
        }
    }
    
    Direction getAlignment() const { return alignment; }
    int getLength() const { return length; }
    int getCompressedLength() const { return compressedLength; }
    bool getIsCompressed() const { return isCompressed; }
    
    void compress(int chars) {
        compressedLength = chars;
        isCompressed = chars > 0;
    }
    
    void release() {
        compressedLength = 0;
        isCompressed = false;
    }
    
    // Check if a position is part of this spring
    bool isPartOfSpring(Point pos) const {
        Point springDir = directionToPoint(alignment);
        for (int i = 0; i < length; i++) {
            Point checkPos = position + Point(springDir.getX() * i, springDir.getY() * i);
            if (checkPos == pos) {
                return true;
            }
        }
        return false;
    }
    
    // Get the index of a position in the spring (0-based, -1 if not part of spring)
    int getPositionIndex(Point pos) const {
        Point springDir = directionToPoint(alignment);
        for (int i = 0; i < length; i++) {
            Point checkPos = position + Point(springDir.getX() * i, springDir.getY() * i);
            if (checkPos == pos) {
                return i;
            }
        }
        return -1;
    }
    
    // Get all positions that make up this spring
    std::vector<Point> getAllPositions() const {
        std::vector<Point> positions;
        Point springDir = directionToPoint(alignment);
        for (int i = 0; i < length; i++) {
            positions.push_back(position + Point(springDir.getX() * i, springDir.getY() * i));
        }
        return positions;
    }
};

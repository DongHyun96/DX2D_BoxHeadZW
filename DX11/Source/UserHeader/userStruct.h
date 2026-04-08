#pragma once

struct CellCoord
{
    int x{};
    int y{};
    
    CellCoord() {}
    CellCoord(UINT x, UINT y) : x(x), y(y) {}
    CellCoord(int x, int y) :x(x), y(y) {}
    
    bool operator==(const CellCoord& other) const
    {
        return (this->x == other.x && this->y == other.y);
    }

    bool operator!=(const CellCoord& other) const
    {
        return (this->x != other.x || this->y != other.y);
    }
    
    CellCoord operator+(const CellCoord& other) const
    {
        return CellCoord(this->x + other.x, this->y + other.y);
    }
    
    CellCoord operator-(const CellCoord& other) const
    {
        return CellCoord(this->x - other.x, this->y - other.y);
    }

    bool operator<(const CellCoord& other) const
    {
        if (this->x != other.x) return this->x < other.x;
        return this->y < other.y;
    }
};

#pragma once

#include <Rtypes.h>

class DataObject
{
protected:
    Float_t X, Y, Z; // Coordinates
public:
    virtual Float_t getX() const = 0;
    virtual Float_t getY() const = 0;
    virtual Float_t getZ() const = 0;

    DataObject(Float_t x, Float_t y, Float_t z)
    {
        X = x;
        Y = y;
        Z = z;
    };
    virtual ~DataObject(){};
};
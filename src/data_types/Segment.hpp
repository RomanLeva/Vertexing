#pragma once

#include "DataObject.hpp"

#include <Rtypes.h>

/** @brief Segment of a Track */
class Segment : public DataObject
{
private:
    Float_t tanX, tanY, tanZ = 1; // direction tangents
    ULong_t trackId;
    void operator delete( void * ) {}
public:
    Float_t getX() const { return X; }
    Float_t getY() const { return Y; }
    Float_t getZ() const { return Z; }
    
    Segment(ULong_t trackId, Float_t x, Float_t y, Float_t z, Float_t tanX, Float_t tanY, Float_t tanZ = 1) : DataObject(x, y, z) 
    {
        this->trackId = trackId;
        this->tanX = tanX;
        this->tanY = tanY;
        this->tanZ = tanZ;
    }

    virtual ~Segment() {}
};
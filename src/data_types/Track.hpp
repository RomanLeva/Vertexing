#pragma once

#include "DataObject.hpp"
#include "Segment.hpp"

#include <Rtypes.h>
#include <algorithm>
#include <stdexcept>

/** @brief Particle Track */
class Track : public DataObject
{
private:
    std::vector<Segment> segments; // track segments
    ULong_t index = 0;
    Float_t tanX = 0, tanY = 0, tanZ = 1; // direction tangents
    Bool_t excluded = false;
    void operator delete( void * ) {} 
public:
    ULong_t getIndex() { return index; }
    Float_t getX() const { return X; }
    Float_t getY() const { return Y; }
    Float_t getZ() const { return Z; }
    Float_t getTanX() const { return tanX; }
    Float_t getTanY() const { return tanY; }
    Float_t getTanZ() const { return tanZ; }

    Bool_t isExcluded() { return excluded; }
    void setAsExcluded() { excluded = true; }
    void setAsIncluded() { excluded = false; }

    void addSegment(Segment &segment)
    {
        segments.push_back(segment);
    }

    Segment &getSegment(int index)
    {
        if (segments.size() - 1 < index)
        {
            throw std::out_of_range("ERROR in getting segment from track. Index is bigger than vector size.");
        }
        return segments.at(index);
    }

    int getSegmentsCount() { return segments.size(); }

public:
    Track(ULong_t index, Float_t x, Float_t y, Float_t z, Float_t tanX, Float_t tanY, Float_t tanZ = 1) : DataObject(x, y, z)
    {
        this->index = index;
        this->tanX = tanX;
        this->tanY = tanY;
        this->tanZ = tanZ;
    }

    Track(const Track &track) : DataObject(track.X, track.Y, track.Z)
    {
        index = track.index;
        tanX = track.tanX;
        tanY = track.tanY;
        tanZ = track.tanZ;
        segments = track.segments;
    }

    Track(Track &&track) : DataObject(track.X, track.Y, track.Z)
    {
        index = track.index;
        tanX = track.tanX;
        tanY = track.tanY;
        tanZ = track.tanZ;
        segments = std::move(track.segments);
    }

    Track &operator=(const Track &track)
    {
        index = track.index;
        X = track.X;
        Y = track.Y;
        Z = track.Z;
        tanX = track.tanX;
        tanY = track.tanY;
        tanZ = track.tanZ;
        segments = track.segments;
        return *this;
    }

    Track &operator=(Track &&track)
    {
        index = track.index;
        X = track.X;
        Y = track.Y;
        Z = track.Z;
        tanX = track.tanX;
        tanY = track.tanY;
        tanZ = track.tanZ;
        segments = std::move(track.segments);
        return *this;
    }

    virtual ~Track() {}
};
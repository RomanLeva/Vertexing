#pragma once

#include "DataObject.hpp"
#include "Track.hpp"

#include <Rtypes.h>
#include <stdexcept>
#include <algorithm>

/** @brief Particle interaction Vertex */
class Vertex : public DataObject
{
private:
    ULong_t index = 0;
    std::vector<Track *> daughterTracks; // daughter tracks pointers
    std::vector<Track *> parentTracks;   // parent tracks pointers
    Bool_t indexInited = false;
    Bool_t excluded = false;
    void operator delete(void *) {}

public:
    Float_t getX() const { return X; }
    Float_t getY() const { return Y; }
    Float_t getZ() const { return Z; }
    Bool_t indexIsInited() { return indexInited; }

    ULong_t getIndex() { return index; }
    void setIndex(ULong_t index)
    {
        this->index = index;
        indexInited = true;
    }

    Bool_t isExcluded() { return excluded; }
    void setAsExcluded() { excluded = true; }
    void setAsIncluded() { excluded = false; }

public:
    /** @brief Store the pointer to daughter track. If track already stored - duplicate is ignored.*/
    void addDaughterTrack(Track *track)
    {
        if (std::find(daughterTracks.begin(), daughterTracks.end(), track) == daughterTracks.end())
        {
            daughterTracks.push_back(track);
        }
    }

    /** @brief Store the pointer to parent track. If track already stored - duplicate is ignored.*/
    void addParentTrack(Track *track)
    {
        if (std::find(parentTracks.begin(), parentTracks.end(), track) == parentTracks.end())
        {
            parentTracks.push_back(track);
        }
    }

    void copyTracksArrays(Vertex &vertexToCopyFrom)
    {
        daughterTracks = vertexToCopyFrom.daughterTracks;
        parentTracks = vertexToCopyFrom.parentTracks;
    }

    void moveTracksArrays(Vertex &vertexToCopyFrom)
    {
        daughterTracks = std::move(vertexToCopyFrom.daughterTracks);
        parentTracks = std::move(vertexToCopyFrom.parentTracks);
    }

    Track *getDaughterTrack(u_int index) { return daughterTracks[index]; }

    Track *getParentTrack(u_int index) { return parentTracks[index]; }

    u_int getDaughterTracksCount() const { return daughterTracks.size(); }

    u_int getParentTracksCount() const { return parentTracks.size(); }

    /** @brief Remove daughter track by index. */
    void removeDaughterTrack(u_int index)
    {
        if (daughterTracks.size() - 1 < index)
        {
            throw std::out_of_range("ERROR in removing daughter track. Index is bigger than vector size.");
        }
        daughterTracks.erase(daughterTracks.begin() + index);
    }

    /** @brief Remove daughter track. Returns true if track was found and removed, othervise false. Method is slower than by index.*/
    bool removeDaughterTrack(Track *track)
    {
        auto it = std::find(daughterTracks.begin(), daughterTracks.end(), track);
        if (it != daughterTracks.end())
        {
            daughterTracks.erase(it);
            return true;
        }
        else
        {
            return false;
        }
    }

    /** @brief Remove parent track by index. */
    void removeParentTrack(u_int index)
    {
        if (parentTracks.size() - 1 < index)
        {
            throw std::out_of_range("ERROR in removing parent track. Index is bigger than vector size.");
        }
        parentTracks.erase(parentTracks.begin() + index);
    }

    /** @brief Remove parent track. Returns true if track was found and removed, othervise false. Method is slower than by index.*/
    bool removeParentTrack(Track &track)
    {
        auto i = std::find(parentTracks.begin(), parentTracks.end(), &track);
        if (i != parentTracks.end())
        {
            parentTracks.erase(i);
            return true;
        }
        else
        {
            return false;
        }
    }

public:
    Vertex(Float_t x, Float_t y, Float_t z) : DataObject(x, y, z) {}

    /** @brief Copy constructor will copy all vars and arrays. */
    Vertex(const Vertex &vertex) : DataObject(vertex.X, vertex.Y, vertex.Z)
    {
        this->setIndex(vertex.index);
        daughterTracks = vertex.daughterTracks;
        parentTracks = vertex.parentTracks;
    }

    /** @brief Move constructor will copy all vars and move arrays. */
    Vertex(const Vertex &&vertex) : DataObject(vertex.X, vertex.Y, vertex.Z)
    {
        this->setIndex(vertex.index);
        daughterTracks = std::move(vertex.daughterTracks);
        parentTracks = std::move(vertex.parentTracks);
    }

    Vertex &operator=(const Vertex &vertex)
    {
        this->setIndex(vertex.index);
        X = vertex.X;
        Y = vertex.Y;
        Z = vertex.Z;
        daughterTracks = vertex.daughterTracks;
        parentTracks = vertex.parentTracks;
        return *this;
    }

    Vertex &operator=(const Vertex &&vertex)
    {
        this->setIndex(vertex.index);
        X = vertex.X;
        Y = vertex.Y;
        Z = vertex.Z;
        daughterTracks = std::move(vertex.daughterTracks);
        parentTracks = std::move(vertex.parentTracks);
        return *this;
    }

    virtual ~Vertex() {}
};

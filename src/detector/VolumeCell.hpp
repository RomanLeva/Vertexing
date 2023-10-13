#pragma once

#include "../data_types/Track.hpp"
#include "../data_types/Vertex.hpp"

#include <algorithm>
#include <stdexcept>

const int CELL_ALIGNMENT = 64; // Cell object will by aligned in CPU memory.

/** @brief Volume Cell that stored in detector's volume. It stores different objects as Tracks, Vertexes and so on.*/
class alignas(CELL_ALIGNMENT) VolumeCell
{
private:
    std::vector<Track> tracks;
    std::vector<Vertex> vertexes;

public:
    /** @brief Move Track to volume cell. */
    void addTrack(Track &&track) { tracks.push_back(std::move(track)); }

    /** @brief Copy Track to volume cell. */
    void addTrack( Track &track) { tracks.push_back(track); }

    /** @brief Copy Vertex to volume cell. */
    void addVertex(Vertex &vertex) { vertexes.push_back(vertex); }

    bool deleteVertex(u_long index)
    {
        for (size_t i = 0; i < vertexes.size(); i++)
        {
            if (vertexes[i].getIndex() == index)
            {
                vertexes.erase(vertexes.begin() + i);
                return true;
            }
        }
        return false;
    }

    /** @brief Get Track by its stored number. */
     Track &getTrack(u_int number) { return tracks.at(number); }

    /** @return cell's tracks count. */
    u_int getTracksCount() const { return tracks.size(); }

    /** @brief Get Vertex by its stored number.
     */
    Vertex &getVertex(u_int number) { return vertexes.at(number); }

    /** @return cell's vertexes count. */
    u_int getVertexesCount() const { return vertexes.size(); }

public:
    VolumeCell() {}

    virtual ~VolumeCell() {}
};
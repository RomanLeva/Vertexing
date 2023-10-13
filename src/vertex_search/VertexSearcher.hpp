#pragma once

#include "../data_types/Track.hpp"
#include "../detector/DetectorVolume.hpp"

#include <optional>

class VertexSearcher
{
public:
    /** @brief Searching vertexes. All Tracks are compared with each other if distance between tracks is less than "NEIGHBOR_TRACK_DISTANCE"
     *  and angle less than "iteration_cuts", algorithm will calculate interaction vertexes for both tracks, add to the vertex
     *  pointers on its tracks, search for additional tracks around vertex coordinates. Save vertexes in detector volume object.
     */
    void searchVertexes(DetectorVolume &detectorVolume);

    /** @brief Determine vertex position as the middle of the common perpendicular to the two given tracks lines
     * @param t1 first track
     * @param t2 second track
     * @returns Optional that contains vertex if calculation succed
     */
    std::optional<Vertex> calculateVertexCoordinates( Track &t1,  Track &t2);

    VertexSearcher();

    virtual ~VertexSearcher()
    {
    }
};

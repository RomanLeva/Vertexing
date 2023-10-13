#pragma once
#include "IDownloader.hpp"

#include "../data_types/Track.hpp"
#include "../data_types/Vertex.hpp"
#include "../data_types/Segment.hpp"

class FedraDownloader : public IDownloader
{
private:
    std::vector<Track> tracksVector;
    std::vector<Vertex> vertexesVector;

public:
    /** @brief Download Tracks from file.
     * @param fileName file path.
     */
    std::vector<Track> &downloadTracksFromFile(std::string fileName);

    /** @brief Download Tracks to file.
     * @param fileName file path.
     * @returns true if file was downloaded succesfully.
     */
    bool downloadTracksToFile(std::string fileName, std::vector< Track *> &tracks);

    /** @brief Download Vertexes from file.
     * @param fileName file path.
     */
    virtual std::vector<Vertex> &downloadVertexesFromFile(std::string fileName);

    /** @brief Download Vertexes to file.
     * @param fileName file path.
     * @returns true if file was downloaded succesfully.
     */
    virtual bool downloadVertexesToFile(std::string fileName, std::vector< Vertex *> &vertexes);

public:
    FedraDownloader(){};
    virtual ~FedraDownloader(){};
};
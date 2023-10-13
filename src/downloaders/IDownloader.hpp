#pragma once
#include <string>

#include "../data_types/Track.hpp"
#include "../data_types/Vertex.hpp"

/** @brief Interface of downloader.*/
class IDownloader
{
public:
    /** @brief Download Tracks from file.
     * @param fileName file path.
     */
    virtual std::vector<Track> &downloadTracksFromFile(std::string fileName) = 0;

    /** @brief Download Tracks to file.
     * @param fileName file path.
     * @returns true if file was downloaded succesfully.
     */
    virtual bool downloadTracksToFile(std::string fileName, std::vector< Track *> &tracks) = 0;

    /** @brief Download Vertexes from file.
     * @param fileName file path.
     */
    virtual std::vector<Vertex> &downloadVertexesFromFile(std::string fileName) = 0;

    /** @brief Download Vertexes to file.
     * @param fileName file path.
     * @returns true if file was downloaded succesfully.
     */
    virtual bool downloadVertexesToFile(std::string fileName, std::vector< Vertex *> &vertexes) = 0;

public:
    IDownloader(){};
    virtual ~IDownloader(){};
};

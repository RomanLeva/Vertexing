#pragma once

#include "../data_types/DataObject.hpp"
#include "../data_types/Track.hpp"
#include "../data_types/Vertex.hpp"

#include "VolumeCell.hpp"

#include <optional>
#include <type_traits>

/**
 *  @brief Qubic detector volume represents 3-D array of 3-D cells, where each cell could store 1-D arrays of Tracks, Vertexes and so on.
 * Each cell can store only the data, which coordinates fits the cell coordinates.
 * To get or store data object we must firstly calculate appropiate cell number (linearCellIndex) or get the cell (getCellByObjectCoordinates).
 * Than use this cell (class VolumeCell) object to store the data or search for the data.
 * Data objects coordinates are: X and Y starts from the detector center (with Z=0) and so, could be negative. Z is always positive.
 * All the data coordinates must fit into detector coordinate system.
 */
class DetectorVolume
{
private:
    u_long tracksCount = 0, vertexesCount = 0;

public:
    /**
     * @brief Download tracks to detector with copying.
     */
    void addTracks(std::vector<Track> &unsortedTracks);

    /**
     * @brief Download tracks to detector with moving.
     */
    void addTracks(std::vector<Track> &&unsortedTracks);

    /**
     * @brief Download vertex to detector. Vertex must have no index initialized.
     */
    void addNewUnindexedVertex(Vertex &vertex);

    /**
     * @brief Check if vertex allready present in detector volume.
     */
    bool checkVertexPresenceByCoordinates(float x, float y, float z);

    /**
     * @brief Check if data object coordinates fits in detector volume size.
     */
    bool checkDataObjectInDetectorBounds(DataObject &object);

    /**
     *  @brief Find vertex by it's coordinates.
     * @return std::optional with Vertex if found
     */
    std::optional<Vertex> findVertexByCoordinates(float x, float y, float z);

    /**
     *  @brief Get all the tracks from all the volume.
     */
    std::vector<Track *> getAllTracks();

    /**
     *  @brief Get all the vertexes from all the volume.
     */
    std::vector<Vertex *> getAllVertexes();

    u_long getTracksCount() { return tracksCount; }

    u_long getVertexesCount() { return vertexesCount; }

    u_int getVolumeDimension();

    bool deleteVertex(u_long index, float x, float y, float z);

    /**
     *  @brief Get tracks from the selected sphere.
     * @warning If some object will be added or removed from the detector, this vector of pointers will become invalid! Object addreses becomes shifted.
     */
    std::vector<Track *> getTracksAround(float x, float y, float z, u_int XYdistance, u_int Zdistance, bool antiDuplicateBorder = false, bool withOutExcluded = true);

    /**
     *  @brief Get vertexes from the selected sphere.
     * @warning If some object will be added or removed from the detector, this vector of pointers will become invalid! Object addreses becomes shifted.
     */
    std::vector<Vertex *> getVertexesAround(float x, float y, float z, u_int XYdistance, u_int Zdistance, bool antiDuplicateBorder = false, bool withOutExcluded = true);

public:
    /**
     *  @brief Create qubic detector's volume object. Volume dimension must be dividable by cell dimension for
     * integer number of volume cells.
     */
    DetectorVolume(u_int volumeDimension, u_int cellDimension);

    virtual ~DetectorVolume(){};

    DetectorVolume(const DetectorVolume &) = delete;
    DetectorVolume &operator=(const DetectorVolume &) = delete;
    DetectorVolume(const DetectorVolume &&) = delete;
    DetectorVolume &operator=(const DetectorVolume &&) = delete;
};

#include "DetectorVolume.hpp"

#include <cmath>
#include <map>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <set>
#include <algorithm>
#include <functional>

namespace
{
    std::vector<VolumeCell> cells; // Cells storing all the data objects

    u_int volumeDim = 0, cellDim = 0, cellsInDim = 0, cellsCount = 0, coordinateCorrection = 0;

    const float MAX_REL_DIFF = 6; // Microns, used for comparing Vertex or Track coordinates equality.

    u_long vertexUniqueIndex = 0;

    // ==================================================================================================================

    void testBordersFit(float x, float y, float z)
    {
        if (std::abs(x) > coordinateCorrection || std::abs(y) > coordinateCorrection || z < 0 || z > volumeDim)
        {
            throw std::out_of_range("ERROR - at least one of the data coordinate goes beyond the detector borders.");
        }
    }

    /* Each data (vertex, track and so on) is stored in it's corresponding spatial cell.
    Correspondance is defined by data coordinates, cell coordinates and cell dimension. */
    u_int getLinearCellIndex(float x, float y, float z)
    {
        u_int X = (u_int)std::floor((x + coordinateCorrection) / cellDim);
        u_int Y = (u_int)std::floor((y + coordinateCorrection) / cellDim);
        u_int Z = (u_int)std::floor(z / cellDim);

        return Z * cellsInDim * cellsInDim + Y * cellsInDim + X;
    }

    void getDataObjectsAround(float x, float y, float z, u_int XYdistance, u_int Zdistance, bool withOutExcluded, bool antiDuplicateBorder,
                              std::function<void(VolumeCell &cell, float x, float y, float z, u_int XYdistance, u_int Zdistance)> callBackFunc)
    {
        // Create qubic search border that not go beyond the detector borders and get rid of negative coordinates
        float objectX = x + coordinateCorrection;
        float objectY = y + coordinateCorrection;
        float objectZ = z; // Z is always positive

        // -1 micron used because: 0 cell index is from x=0 to x=cellSize-1, than 1 cell index is from x=cellSize to... and so on
        float Xmin = objectX - XYdistance < 0 ? 0 : objectX - XYdistance;
        float Xmax = objectX + XYdistance >= volumeDim - 1 ? volumeDim - 1 : objectX + XYdistance;

        float Ymin = objectY - XYdistance < 0 ? 0 : objectY - XYdistance;
        float Ymax = objectY + XYdistance >= volumeDim ? volumeDim - 1 : objectY + XYdistance;

        float Zmin = objectZ - Zdistance < 0 ? 0 : objectZ - Zdistance;
        float Zmax = objectZ + Zdistance >= volumeDim ? volumeDim - 1 : objectZ + Zdistance;

        float searchX, searchY, searchZ, algZmin;

        float XYborder = std::sqrt(std::pow(XYdistance, 2) + std::pow(XYdistance, 2));

        if (antiDuplicateBorder)
        {
            searchX = std::floor(objectX / cellDim) * cellDim;
            searchY = std::floor(objectY / cellDim) * cellDim;
            algZmin = searchZ = std::floor(objectZ / cellDim) * cellDim;
        }
        else
        {
            searchX = Xmin;
            searchY = Ymin;
            searchZ = Zmin;
        }

        while (searchX <= Xmax)
        {
            while (searchY <= Ymax)
            {
                while (searchZ <= Zmax)
                {
                    auto searchCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
                    auto &searchCell = cells[searchCellInd];

                    callBackFunc(searchCell, x, y, z, XYdistance, Zdistance);

                    searchZ += cellDim;
                    if (searchZ > Zmax)
                    {
                        searchZ = Zmax;
                        auto nextCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
                        if (nextCellInd == searchCellInd)
                            break;
                    }
                }
                auto searchCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
                searchY += cellDim;
                if (searchY > Ymax)
                {
                    searchY = Ymax;

                    auto nextCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
                    if (nextCellInd == searchCellInd)
                        break;

                    if (antiDuplicateBorder)
                    {
                        objectX - searchX >= 0 ? searchZ = algZmin : searchZ = Zmin;
                    }
                    else
                    {
                        searchZ = Zmin;
                    }
                }
                else
                {
                    searchZ = Zmin;
                }
            }
            auto searchCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
            searchX += cellDim;
            if (searchX > Xmax)
            {
                searchX = Xmax;

                auto nextCellInd = getLinearCellIndex(searchX - coordinateCorrection, searchY - coordinateCorrection, searchZ);
                if (nextCellInd == searchCellInd)
                    break;

                searchZ = Zmin;
                searchY = Ymin;
            }
            else
            {
                searchZ = Zmin;
                searchY = Ymin;
            }
        }
    }

} // ================================== end of file private namespace ==========================================

void DetectorVolume::addTracks(std::vector<Track> &unsortedTracks) // copy
{
    for (Track &track : unsortedTracks)
    {
        testBordersFit(track.getX(), track.getY(), track.getZ());

        auto cellInd = getLinearCellIndex(track.getX(), track.getY(), track.getZ());
        auto &cell = cells[cellInd];
        cell.addTrack(track);
        tracksCount++;
    }
}

void DetectorVolume::addTracks(std::vector<Track> &&unsortedTracks) // move
{
    for (Track &track : unsortedTracks)
    {
        testBordersFit(track.getX(), track.getY(), track.getZ());

        auto cellInd = getLinearCellIndex(track.getX(), track.getY(), track.getZ());
        auto &cell = cells[cellInd];
        cell.addTrack(std::move(track));
        tracksCount++;
    }
}

void DetectorVolume::addNewUnindexedVertex(Vertex &vertex)
{

    testBordersFit(vertex.getX(), vertex.getY(), vertex.getZ());
    if (!vertex.indexIsInited())
    {

        vertex.setIndex(vertexUniqueIndex++);
        auto cellInd = getLinearCellIndex(vertex.getX(), vertex.getY(), vertex.getZ());
        auto &cell = cells[cellInd];
        cell.addVertex(vertex);
        vertexesCount++;
    }
    else
    {
        auto cellInd = getLinearCellIndex(vertex.getX(), vertex.getY(), vertex.getZ());
        auto &cell = cells[cellInd];
        for (size_t i = 0; i < cell.getVertexesCount(); i++)
        {
            auto cellVertex = cell.getVertex(i);
            if (cellVertex.getIndex() == vertex.getIndex())
            {
                if (&cellVertex == &vertex)
                    return;

                cell.deleteVertex(i);
                break;
            }
        }
        cell.addVertex(vertex);
    }
}

bool DetectorVolume::checkVertexPresenceByCoordinates(float x, float y, float z)
{
    testBordersFit(x, y, z);

    auto lin = getLinearCellIndex(x, y, z);
    auto &cell = cells[lin];

    if (cell.getVertexesCount() > 0)
    {
        for (u_int i = 0; i < cell.getVertexesCount(); i++)
        {
            auto &vertex = cell.getVertex(i);

            if (std::abs(vertex.getX() - x) < MAX_REL_DIFF && std::abs(vertex.getY() - y) < MAX_REL_DIFF && std::abs(vertex.getZ() - z) < MAX_REL_DIFF)
            {
                return true;
            }
        }
    }
    return false;
}

bool DetectorVolume::checkDataObjectInDetectorBounds(DataObject &object)
{
    if (std::abs(object.getX()) < coordinateCorrection && std::abs(object.getY()) < coordinateCorrection && object.getZ() < volumeDim && object.getZ() >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<Vertex> DetectorVolume::findVertexByCoordinates(float x, float y, float z)
{
    testBordersFit(x, y, z);

    auto lin = getLinearCellIndex(x, y, z);
    auto &cell = cells[lin];

    if (cell.getVertexesCount() > 0)
    {
        for (u_int i = 0; i < cell.getVertexesCount(); i++)
        {
            auto &vertex = cell.getVertex(i);

            if (std::abs(vertex.getX() - x) < MAX_REL_DIFF && std::abs(vertex.getY() - y) < MAX_REL_DIFF && std::abs(vertex.getZ() - z) < MAX_REL_DIFF)
            {
                return vertex;
            }
        }
    }
    return std::nullopt;
}

std::vector<Track *> DetectorVolume::getAllTracks()
{
    std::vector<Track *> objectsToReturn;
    for (size_t c = 0; c < cellsCount; c++)
    {
        auto &cell = cells[c];
        for (u_int t = 0; t < cell.getTracksCount(); t++)
        {
            objectsToReturn.emplace_back(&cell.getTrack(t));
        }
    }
    return objectsToReturn;
}

std::vector<Vertex *> DetectorVolume::getAllVertexes()
{
    std::vector<Vertex *> vertexes;
    for (size_t c = 0; c < cellsCount; c++)
    {
        auto &cell = cells[c];
        for (u_int v = 0; v < cell.getVertexesCount(); v++)
        {
            auto &vert = cell.getVertex(v);
            vertexes.emplace_back(&vert);
        }
    }
    return vertexes;
}

u_int DetectorVolume::getVolumeDimension()
{
    return volumeDim;
}

bool DetectorVolume::deleteVertex(u_long index, float x, float y, float z)
{
    auto cellInd = getLinearCellIndex(x, y, z);
    auto &cell = cells[cellInd];
    return cell.deleteVertex(index);
}

std::vector<Track *> DetectorVolume::getTracksAround(float x, float y, float z, u_int XYdistance, u_int Zdistance, bool withOutExcluded, bool antiDuplicateBorder)
{
    testBordersFit(x, y, z);

    std::vector<Track *> objectsToReturn;

    auto getObjectsLambda = [&objectsToReturn](VolumeCell &searchCell, float x, float y, float z, u_int XYdistance, u_int Zdistance)
    {
        for (size_t i = 0; i < searchCell.getTracksCount(); i++)
        {
            auto &obj = searchCell.getTrack(i);
            if(obj.isExcluded())
                continue;

            auto XYdelta = std::sqrt(std::pow(obj.getX() - x, 2) + std::pow(obj.getY() - y, 2));
            auto Zdelta = obj.getZ() - z;

            if (XYdelta <= XYdistance && Zdelta <= Zdistance)
            {
                objectsToReturn.emplace_back(&searchCell.getTrack(i));
            }
        }
    };

    getDataObjectsAround(x, y, z, XYdistance, Zdistance, withOutExcluded, antiDuplicateBorder, getObjectsLambda);
    return objectsToReturn;
}

std::vector<Vertex *> DetectorVolume::getVertexesAround(float x, float y, float z, u_int XYdistance, u_int Zdistance, bool withOutExcluded, bool antiDuplicateBorder)
{
    testBordersFit(x, y, z);

    std::vector<Vertex *> objectsToReturn;

    auto getObjectsLambda = [&objectsToReturn](VolumeCell &searchCell, float x, float y, float z, u_int XYdistance, u_int Zdistance)
    {
        for (size_t i = 0; i < searchCell.getVertexesCount(); i++)
        {
            auto &obj = searchCell.getVertex(i);
            if(obj.isExcluded())
                continue;

            auto XYdelta = std::sqrt(std::pow(obj.getX() - x, 2) + std::pow(obj.getY() - y, 2));
            auto Zdelta = obj.getZ() - z;

            if (XYdelta <= XYdistance && Zdelta <= Zdistance)
            {
                objectsToReturn.emplace_back(&searchCell.getVertex(i));
            }
        }
    };

    getDataObjectsAround(x, y, z, XYdistance, Zdistance, withOutExcluded, antiDuplicateBorder, getObjectsLambda);
    return objectsToReturn;
}

DetectorVolume::DetectorVolume(u_int volumeDimension, u_int cellDimension)
{
    if (volumeDimension % cellDimension != 0)
    {
        std::__throw_invalid_argument("ERROR arguments: volumeDim must be dividable by cellDim!");
    }
    volumeDim = volumeDimension;
    cellDim = cellDimension;
    cellsInDim = volumeDim / cellDim;
    cellsCount = cellsInDim * cellsInDim * cellsInDim;

    // Move the coordinate system to get rid of negative XY coordinates, Z is always positive.
    coordinateCorrection = volumeDim / 2;

    cells.resize(cellsCount);

    // posix_memalign((void **)cellsArray, CELL_ALIGNMENT, cellsCount * sizeof(VolumeCell));
    // for (size_t i = 0; i < cellsCount; i++)
    // {
    //     ::new (&cellsArray[i]) VolumeCell(); // execute constructor on the piece of memory
    // }
}
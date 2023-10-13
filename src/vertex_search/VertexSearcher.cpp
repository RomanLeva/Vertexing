#include "VertexSearcher.hpp"

#include "../downloaders/FedraDownloader.hpp"
#include "../data_types/DataObject.hpp"
#include "../data_types/Track.hpp"
#include "../data_types/Vertex.hpp"
#include "../utility/CalculationAndAlgorithms.hpp"

#include <unordered_set>
#include <cmath>
#include <functional>
#include <immintrin.h>
#include <iostream>

#include <TMinuit.h>

using namespace std;

namespace
{
    const float NEIGHBOR_TRACK_XY_DISTANCE = 1000; // microns
    const float NEIGHBOR_TRACK_Z_DISTANCE = 100;   // microns
    const float VERTEX_TO_TRACK_XY_DIST = 1000;    // microns
    const float VERTEX_TO_TRACK_Z_DIST = 1000;     // microns
    const float IMPACT_PARAMETER = 15;             // microns
    const float TRACKS_PERPENDICULAR = 10;         // microns
    const float DIRECT_TRACK_ANGLE = 0.02;         // radians
    const int DAUGHTERS_COUNT_CUT = 4;
    const int MINUIT_ITERATIONS = 10;      // TMinuit minimization iterations
    const float VERTEX_CLOSE_BY_X_Y = 100; // microns
    const float VERTEX_CLOSE_BY_Z = 600;   // microns
    const bool PRINT_VERT_STAT = true;

    Vertex *vertex_ptr = nullptr;

    std::unique_ptr<TMinuit> minuit;

    bool checkVertexAndDaughterTracksCuts(Vertex &vertex, Track *track1, Track *track2)
    {
        float tr1z = track1->getZ();
        float tr2z = track2->getZ();
        if (vertex.getZ() < tr1z - VERTEX_TO_TRACK_Z_DIST | vertex.getZ() < tr2z - VERTEX_TO_TRACK_Z_DIST)
        {
            return false;
        }
        if (vertex.getZ() > tr1z + VERTEX_TO_TRACK_Z_DIST | vertex.getZ() > tr2z + VERTEX_TO_TRACK_Z_DIST)
        {
            return false;
        }
        return true;
    }

    /* Checks if vertex positions are close enough so that vertices can be considered as one.
     * Returns true if vertices should be united, false otherwise.
     */
    bool checkIfVerticesAreClose(Vertex &vertex_1, Vertex &vertex_2)
    {
        float dx = abs(vertex_1.getX() - vertex_2.getX());
        float dy = abs(vertex_1.getY() - vertex_2.getY());
        float dz = abs(vertex_1.getZ() - vertex_2.getZ());

        return dx * dx + dy * dy < VERTEX_CLOSE_BY_X_Y * VERTEX_CLOSE_BY_X_Y && dz < VERTEX_CLOSE_BY_Z;
    }

    bool checkTrackAngleLessThanCut(Track *track, float angleCut)
    {
        return atan(sqrt(track->getTanX() * track->getTanX() + track->getTanY() * track->getTanY())) < angleCut;
    }

    /* Sum of all impact parameters to minimize when recalculating vertex position, default TMinuit function signature
     */
    void FCN(Int_t &npar, Double_t *gin, Double_t &f, Double_t *coordinate, Int_t iflag)
    {
        if (vertex_ptr == nullptr)
        {
            std::cerr << "ERROR: Vertex poiner used in Minuit FCN is null pointer.";
        }

        Vertex vertex(coordinate[0], coordinate[1], coordinate[2]);
        f = 0;

        for (int t = 0; t < vertex_ptr->getDaughterTracksCount(); t++)
        {
            auto track = vertex_ptr->getDaughterTrack(t);

            f += CalculationAndAlgorithms::calculateImpactParameter(vertex, track);
        }
    }

    /* Recalculating vertex position by finding minimum of the sum of impact parameters.
    If vertex gone out of detector borders - returns false. If borders fits - returns true.
     */
    std::optional<Vertex> recalculateVertexPosition(DetectorVolume &detectorVolume, Vertex &vertex, Double_t &error, int iterations)
    {
        vertex_ptr = &vertex;

        Int_t ierflg;

        minuit->mnparm(0, "x", vertex.getX(), 1, 0, 0, ierflg);
        minuit->mnparm(1, "y", vertex.getY(), 1, 0, 0, ierflg);
        minuit->mnparm(2, "z", vertex.getZ(), 1, 0, 0, ierflg);

        Double_t arglist[10];
        arglist[0] = iterations;

        minuit->mnexcm("MIGRAD", arglist, 1, ierflg);

        double newX;
        double newY;
        double newZ;

        minuit->GetParameter(0, newX, error);
        minuit->GetParameter(1, newY, error);
        minuit->GetParameter(2, newZ, error);

        Vertex newVertex((float)newX, (float)newY, (float)newZ);

        if (!detectorVolume.checkDataObjectInDetectorBounds(newVertex))
        {
            return std::nullopt;
        }

        float oldX = round(vertex.getX());
        float oldY = round(vertex.getY());
        float oldZ = round(vertex.getZ());
        float newXf = round(newX);
        float newYf = round(newY);
        float newZf = round(newZ);

        // if (oldX != newXf | oldY != newYf | oldZ != newZf) // vertex moved more than 1 micron
        // {
        //     printf("OLD %g %g %g\n", oldX, oldY, oldZ);
        //     printf("NEW %g %g %g\n", newXf, newYf, newZf);
        //     printf("\n");
        //     static int count = 0;
        //     printf("Moved more than 1mu %i \n", count++);
        // }
        return newVertex;
    }
}

std::optional<Vertex> VertexSearcher::calculateVertexCoordinates(Track &t1, Track &t2)
{
    __m256 coord1 = _mm256_set_ps(0, 0, 0, 0, 0, t1.getZ(), t1.getY(), t1.getX());
    __m256 coord2 = _mm256_set_ps(0, 0, 0, 0, 0, t2.getZ(), t2.getY(), t2.getX());
    __m256 dir1 = _mm256_set_ps(0, 0, 0, 0, 0, t1.getTanZ(), t1.getTanY(), t1.getTanX());
    __m256 dir2 = _mm256_set_ps(0, 0, 0, 0, 0, t2.getTanZ(), t2.getTanY(), t2.getTanX());

    auto pointDist = _mm256_sub_ps(coord2, coord1);
    auto mixed = std::abs(CalculationAndAlgorithms::mixedProduct(pointDist, dir1, dir2));
    auto dirDot = CalculationAndAlgorithms::crossProduct(dir1, dir2);
    auto dirDotMagn = CalculationAndAlgorithms::vectorMagnitude(dirDot);
    auto perpendicular = mixed / dirDotMagn;
    float perpFloored = floor(perpendicular * 100) / 100;

    if (perpendicular > TRACKS_PERPENDICULAR)
        return std::nullopt;

    __m256 vec1general = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[0], -t1.getTanX(), t2.getTanX());
    __m256 vec2general = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[1], -t1.getTanY(), t2.getTanY());
    __m256 vec3general = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[2], -t1.getTanZ(), t2.getTanZ());
    auto detGeneral = CalculationAndAlgorithms::mixedProduct(vec1general, vec2general, vec3general);

    __m256 vec1first = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[0], -t1.getTanX(), t1.getX() - t2.getX());
    __m256 vec2first = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[1], -t1.getTanY(), t1.getY() - t2.getY());
    __m256 vec3first = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[2], -t1.getTanZ(), t1.getZ() - t2.getZ());
    auto detFirst = CalculationAndAlgorithms::mixedProduct(vec1first, vec2first, vec3first);

    __m256 vec1second = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[0], t1.getX() - t2.getX(), t2.getTanX());
    __m256 vec2second = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[1], t1.getY() - t2.getY(), t2.getTanY());
    __m256 vec3second = _mm256_set_ps(0, 0, 0, 0, 0, -CalculationAndAlgorithms::crossProduct(dir1, dir2)[2], t1.getZ() - t2.getZ(), t2.getTanZ());
    auto detSecond = CalculationAndAlgorithms::mixedProduct(vec1second, vec2second, vec3second);

    auto variableS = detFirst / detGeneral;
    auto variableT = detSecond / detGeneral;

    Vertex vertex(floor(((t2.getTanX() * variableS + t2.getX() + t1.getTanX() * variableT + t1.getX()) / 2) * 100) / 100,
                  floor(((t2.getTanY() * variableS + t2.getY() + t1.getTanY() * variableT + t1.getY()) / 2) * 100) / 100,
                  floor(((t2.getTanZ() * variableS + t2.getZ() + t1.getTanZ() * variableT + t1.getZ()) / 2) * 100) / 100);

    return vertex;
}

void VertexSearcher::searchVertexes(DetectorVolume &detectorVolume)
{
    u_long vertexDuplicate = 0;
    u_long noVertexCount = 0;
    u_long trackEqlsNeighbor = 0;
    u_long vertexOutOfBounds = 0;
    u_long vertexAlongFromTracks = 0;
    u_long excludedTrackTouched = 0;

    for (auto track : detectorVolume.getAllTracks())
    {
        auto neighborTracks = detectorVolume.getTracksAround(track->getX(), track->getY(), track->getZ(), NEIGHBOR_TRACK_XY_DISTANCE, NEIGHBOR_TRACK_Z_DISTANCE, true, false);

        for (auto neighborTrack : neighborTracks)
        {
            if (neighborTrack->isExcluded())
            {
                excludedTrackTouched++;
                continue;
            }

            if (neighborTrack == track)
            {
                trackEqlsNeighbor++;
                continue;
            }

            auto vertexOpt = calculateVertexCoordinates(*track, *neighborTrack);

            if (!vertexOpt.has_value())
            {
                noVertexCount++;
                continue;
            }

            auto vertex = vertexOpt.value();
            if (!detectorVolume.checkDataObjectInDetectorBounds(vertex))
            {
                vertexOutOfBounds++;
                continue;
            }

            if (!checkVertexAndDaughterTracksCuts(vertex, track, neighborTrack))
            {
                vertexAlongFromTracks++;
                continue;
            }

            if (detectorVolume.checkVertexPresenceByCoordinates(vertex.getX(), vertex.getY(), vertex.getZ())) // vertex was allready found before
            {
                vertexDuplicate++;
                continue;
            }
            track->setAsExcluded();
            neighborTrack->setAsExcluded();

            auto moreNeighborTracks = detectorVolume.getTracksAround(vertex.getX(), vertex.getY(), vertex.getZ(), VERTEX_TO_TRACK_XY_DIST, VERTEX_TO_TRACK_Z_DIST, true, false);

            for (auto moreTrack : moreNeighborTracks)
            {
                if (moreTrack->isExcluded())
                    continue;
                if (moreTrack->getZ() < vertex.getZ())
                    continue;

                if (CalculationAndAlgorithms::calculateImpactParameter(vertex, moreTrack) < IMPACT_PARAMETER)
                {
                    moreTrack->setAsExcluded();
                    vertex.addDaughterTrack(moreTrack);
                }
            }

            vertex.addDaughterTrack(track);
            vertex.addDaughterTrack(neighborTrack);

            detectorVolume.addNewUnindexedVertex(vertex);
        }
    }

    struct VertexStruct
    {
        u_long index;
        float X, Y, Z;
    };

    std::vector<VertexStruct> vertexesToDelete;
    std::vector<Vertex> vertexesToAdd;

    for (auto vertex : detectorVolume.getAllVertexes())
    {
        if (vertex->getDaughterTracksCount() <= 2)
            continue;
        Double_t error = 0;
        auto optVertex = recalculateVertexPosition(detectorVolume, *vertex, error, MINUIT_ITERATIONS);
        if (optVertex.has_value())
        {
            auto newVertex = optVertex.value();
            newVertex.copyTracksArrays(*vertex);
            newVertex.setIndex(vertex->getIndex());

            vertexesToDelete.push_back({vertex->getIndex(), vertex->getX(), vertex->getY(), vertex->getZ()});
            vertexesToAdd.push_back(newVertex);
        }
    }

    for (auto vertex : vertexesToDelete)
    {
        detectorVolume.deleteVertex(vertex.index, vertex.X, vertex.Y, vertex.Z);
    }
    for (auto vertex : vertexesToAdd)
    {
        detectorVolume.addNewUnindexedVertex(vertex);
    }
    vertexesToAdd.clear();
    vertexesToDelete.clear();

    for (auto vertex : detectorVolume.getAllVertexes())
    {
        auto moreNeighborTracks = detectorVolume.getTracksAround(vertex->getX(), vertex->getY(), vertex->getZ(), VERTEX_TO_TRACK_XY_DIST, VERTEX_TO_TRACK_Z_DIST, true, false);

        for (auto moreTrack : moreNeighborTracks)
        {
            if (moreTrack->isExcluded())
                continue;

            if (CalculationAndAlgorithms::calculateImpactParameter(*vertex, moreTrack) < IMPACT_PARAMETER)
            {
                moreTrack->setAsExcluded();
                vertex->addDaughterTrack(moreTrack);
            }
        }
    }
    printf("noVertexCount=%li vertexDuplicates=%li vertexAlongFromTracks=%li vertexOutOfBounds=%li trackEqlsNeighbor=%li excludedTrackTouched=%li\n",
           noVertexCount, vertexDuplicate, vertexAlongFromTracks, vertexOutOfBounds, trackEqlsNeighbor, excludedTrackTouched);

    for (auto vertex : detectorVolume.getAllVertexes())
    {
        if (vertex->getDaughterTracksCount() < DAUGHTERS_COUNT_CUT)
        {
            vertexesToDelete.push_back({vertex->getIndex(), vertex->getX(), vertex->getY(), vertex->getZ()});
        }
    }
    for (auto vertex : vertexesToDelete)
    {
        detectorVolume.deleteVertex(vertex.index, vertex.X, vertex.Y, vertex.Z);
    }
    vertexesToDelete.clear();
    printf("Deleted vertexes with daughter tracks count < %i . \n", DAUGHTERS_COUNT_CUT);
}

VertexSearcher::VertexSearcher()
{
    minuit = std::make_unique<TMinuit>(3);
    minuit->SetFCN(FCN);
    minuit->SetPrintLevel(-1);
}
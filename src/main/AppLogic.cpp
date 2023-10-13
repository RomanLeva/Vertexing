#include "AppLogic.hpp"

#include <string>
#include <memory>
#include <chrono>

#include "../detector/DetectorVolume.hpp"
#include "../downloaders/FedraDownloader.hpp"
#include "../utility/CalculationAndAlgorithms.hpp"
#include "../utility/Histograming.hpp"
#include "../vertex_search/VertexSearcher.hpp"
#include "../vertex_processing/VertexProcessor.hpp"
#include "../data_types/DataObject.hpp"
#include "../data_types/Track.hpp"

#include "TApplication.h"

namespace
{
    // ================= PARAMETERS ======================================================================

    const std::string TRACKS_FILE_NAME = "~/Vertexing/resources/downloaded_tracks.root";
    const std::string VERTEXES_ROOT_FILE_NAME = "~/Vertexing/vertexes.root";
    const std::string VERTEXES_TEXT_FILE_NAME = "processed_vertexes.txt"; // file will be created in project build directory
    const int VOLUME_DIMENSION = 20000;                                   // microns
    const float STRAIGHT_TRACK_ANGLE_CUT = 0.02;                          // radian
    const bool HISTOGRAMING = true;
    const bool CUT_DIRECT_TRACKS = true;

    // ===================================================================================================

    // ================ STATIC DEPENDENCIES =====================================================================

    std::unique_ptr<DetectorVolume> detectorVolume;
    CalculationAndAlgorithms calculationAlgorithms;
    Histograming histograming;
    VertexSearcher vertexSearcher;
    //VertexProcessor vertexProcessor;
    TApplication rootApp("DsTauVertexing", 0, 0);

    // ===================================================================================================

    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> start_time;
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> end_time;

    void startTimer(std::string message)
    {
        start_time = std::chrono::high_resolution_clock::now();
        printf("%s \n", message.c_str());
    }
    void stopTimer(std::string message)
    {
        end_time = std::chrono::high_resolution_clock::now();
        auto measured_time = end_time - start_time;
        printf("%s in %lu ms \n", message.c_str(), measured_time / std::chrono::milliseconds(1));
    }

    bool checkTrackAngleLessThanCut(Track *track, float angleCut)
    {
        return atan(sqrt(track->getTanX() * track->getTanX() + track->getTanY() * track->getTanY())) < angleCut;
    }
}

void AppLogic::findVertexes()
{
    startTimer("Start downloading from file...");
    auto &tracks = downloader->downloadTracksFromFile(TRACKS_FILE_NAME); // download tracks from linked_tracks file

    std::string trackVectorSize = std::to_string(tracks.size());
    std::string downloadRes = "Downloaded " + trackVectorSize + " tracks from linked_tracks.root";
    stopTimer(downloadRes);

    auto tracksCount = tracks.size();
    auto cellSize = calculationAlgorithms.calculateCellSizeFromTracksCount(VOLUME_DIMENSION, tracksCount);

    if (!detectorVolume)
        detectorVolume = std::make_unique<DetectorVolume>(VOLUME_DIMENSION, cellSize);
    printf("Created detector volume with cell of size %i microns. \n", cellSize);

    std::vector<Track> withStraightTracksExcluded;
    std::vector<Track> tracksStraightLeft;
    if (CUT_DIRECT_TRACKS)
    {
        for (auto track : tracks)
        {
            if (!checkTrackAngleLessThanCut(&track, STRAIGHT_TRACK_ANGLE_CUT))
            {
                withStraightTracksExcluded.emplace_back(track);
            }
            else
            {
                tracksStraightLeft.emplace_back(track);
            }
        }
        printf("Straight tracks with angle < %g were excluded from search. %li tracks left. \n", STRAIGHT_TRACK_ANGLE_CUT, withStraightTracksExcluded.size());

        startTimer("Start downloading to detector object...");
        detectorVolume->addTracks(std::move(withStraightTracksExcluded)); // move to detector volume object
        stopTimer("Tracks were downloaded to in memory detector volume object");
    }
    else
    {
        startTimer("Start downloading to detector object...");
        detectorVolume->addTracks(std::move(tracks)); // move to detector volume object
        stopTimer("Tracks were downloaded to in memory detector volume object");
    }

    printf("\n");

    startTimer("Start searching vertexes...");
    vertexSearcher.searchVertexes(*detectorVolume.get()); // <<<====================== search vertexes

    std::string searchRes = "Searching vertexes succesfully finished. ";
    auto vertexesVecBefore = detectorVolume->getAllVertexes();
    std::string vertexesSizeStr = std::to_string(vertexesVecBefore.size());
    std::string resBefore = searchRes + ", vertexes count " + vertexesSizeStr + " pieces";
    stopTimer(resBefore);

    auto vertexPtrs = detectorVolume->getAllVertexes();

    if (HISTOGRAMING)
    {
        histograming.checkVertexesWithHistograms(detectorVolume->getAllVertexes());
        rootApp.Run();
    }

    printf("Processing is over. \n");
    printf("\n");

    startTimer("Start downloading to file... ");
    auto succesfullyDownloaded = downloader->downloadVertexesToFile(VERTEXES_TEXT_FILE_NAME, vertexPtrs);
    if (succesfullyDownloaded)
    {
        stopTimer("Vertexes file has been created in the project build directory ");
    }
    else
    {
        stopTimer("ERROR in vertexes file writing!!!");
    }

    if (CUT_DIRECT_TRACKS)
    {
        detectorVolume->addTracks(tracksStraightLeft);
        printf("Straight tracks was added back to the detector volume. \n");
    }
}

AppLogic::AppLogic(std::unique_ptr<IDownloader> downloader)
{
    this->downloader = std::move(downloader);
}

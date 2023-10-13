#include "VertexProcessor.hpp"

#include "../detector/DetectorVolume.hpp"
#include "../utility/CalculationAndAlgorithms.hpp"
#include "../data_types/Vertex.hpp"

#include <TMVA/Tools.h>
#include <TMVA/Reader.h>
#include <TMVA/MethodCuts.h>

#include <unordered_set>
#include <optional>

namespace
{
    const float DZ_BARIER = 1000;       // micron
    const float VERTEX_DX_DY_CUT = 600; // microns
    const float BDT_cut = 0.6277;
    const float BDT_cut_02 = BDT_cut - 0.2;
    const float VERTEX_CLOSE_BY_X_Y = 100; // microns
    const float VERTEX_CLOSE_BY_Z = 600;   // microns
    const float MAX_IMPACT_PAR = 5;        // microns
    const int MINUIT_ITERATIONS = 100;

    std::unique_ptr<TMVA::Reader> reader;

    float Zdifference;
    float average[2];
    float numberOfDaughters;
    float chi2;
    float positionZ;
}

void VertexProcessor::processVertexesWithML(DetectorVolume &detectorVolume)
{
    std::vector<Vertex *> vertexesToDelete;
    std::vector<Vertex> vertexesToAdd;

    std::map<u_long, std::unordered_set<u_long>> daughterToVertex;
    std::map<u_long, Vertex *> indexesOfVertexes;

    for (auto vertex : detectorVolume.getAllVertexes())
    {
        indexesOfVertexes[vertex->getIndex()] = vertex;

        for (size_t t = 0; t < vertex->getDaughterTracksCount(); t++)
        {
            auto track = vertex->getDaughterTrack(t);
            daughterToVertex[track->getIndex()].insert(vertex->getIndex());
        }
    }

    for (auto vertex : detectorVolume.getAllVertexes())
    {
        Vertex newVertex = *vertex;
        vertexesToDelete.push_back(vertex);

        chi2 = CalculationAndAlgorithms::calculateChiSquared(newVertex);
        numberOfDaughters = (float)newVertex.getDaughterTracksCount();
        positionZ = (float)newVertex.getZ();

        Zdifference = 0;
        average[0] = 0;
        average[1] = 0;

        for (int t = 0; t < newVertex.getDaughterTracksCount(); t++)
        {
            auto track = newVertex.getDaughterTrack(t);
            Zdifference += (float)track->getZ();
            average[0] += (float)track->getTanX();
            average[1] += (float)track->getTanY();
        }

        Zdifference /= numberOfDaughters;
        average[0] /= numberOfDaughters;
        average[1] /= numberOfDaughters;
        Zdifference -= positionZ;

        auto MVA_value = reader->EvaluateMVA("BDTG");

        if (MVA_value < BDT_cut && newVertex.getDaughterTracksCount() <= 4)
        {
            vertexesToAdd.push_back(newVertex);
        }
    }
    for (auto vertex : vertexesToDelete)
    {
        detectorVolume.deleteVertex(vertex->getIndex(), vertex->getX(), vertex->getY(), vertex->getZ());
    }
    for (auto vertex : vertexesToAdd)
    {
        detectorVolume.addNewUnindexedVertex(vertex);
    }
}

VertexProcessor::VertexProcessor()
{

    reader = std::make_unique<TMVA::Reader>("!Color:!Silent");
    reader->AddVariable("chi2", &chi2);
    reader->AddVariable("position[2]", &positionZ);
    reader->AddVariable("numberOfDaughters", &numberOfDaughters);
    reader->AddVariable("avg_dau_pos[2] - position[2]", &Zdifference);
    reader->AddVariable("average[0]", &average[0]);
    reader->AddVariable("average[1]", &average[1]);

    //reader->BookMVA("BDTG", "../weights/TMVAClassificationCategory_BDTG.weights.xml");
}

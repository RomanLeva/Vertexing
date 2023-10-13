#pragma once

#include "../detector/DetectorVolume.hpp"

class VertexProcessor
{
public:

    void processVertexesWithML(DetectorVolume &detectorVolume);

    VertexProcessor();
    virtual ~VertexProcessor() {};
};



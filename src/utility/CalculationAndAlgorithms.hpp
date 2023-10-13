#pragma once

#include "../data_types/Vertex.hpp"

#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>

#include <cmath>
#include <immintrin.h>

class CalculationAndAlgorithms
{
public:
    static u_int calculateCellSizeFromTracksCount(u_int volumeDimension, u_int tracksCount)
    {
        u_int cellDim = 0;

        long volume = std::pow(volumeDimension, 3);
        double volumeDensity = (double)tracksCount / volume;
        float oneTrackVolume = 1 / volumeDensity; // average volume needed for one track
        u_int avgCellDim = (u_int)std::round(std::cbrt(oneTrackVolume));
        u_int cicle = 0;

        // Search for the volume cell dimension. Volume must be dividable by cell.
        do
        {
            u_int min = avgCellDim - cicle;
            u_int max = avgCellDim + cicle;
            u_int minMod = volumeDimension % min;
            u_int maxMod = volumeDimension % max;
            if (minMod == 0)
            {
                cellDim = min;
                break;
            }
            if (maxMod == 0)
            {
                cellDim = max;
                break;
            }
            cicle++;
        } while (cicle != 500);

        if (cicle == 500 & tracksCount < 1500L)
        {
            cellDim = volumeDimension;
        }

        if (cicle == 500 & tracksCount > 1500L)
        {
            std::__throw_invalid_argument("Error arguments: could not find optimal cell dimension, \
                because the volume dimension parameter may by simple number.");
        }

        return cellDim;
    }

    static __m256 crossProduct(__m256 vec1, __m256 vec2)
    {
        __m256 shuffle1 = _mm256_permute_ps(vec2, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 shuffle2 = _mm256_permute_ps(vec1, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 mul1 = _mm256_mul_ps(shuffle1, shuffle2);

        shuffle1 = _mm256_permute_ps(vec2, _MM_SHUFFLE(3, 1, 0, 2));
        shuffle2 = _mm256_permute_ps(vec1, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 mul2 = _mm256_mul_ps(shuffle1, shuffle2);

        __m256 result = _mm256_sub_ps(mul2, mul1);

        return result;
    }

    static __m256 crossProduct(const float *vector1, const float *vector2)
    {
        __m256 v1 = _mm256_set_ps(0, 0, 0, 0, 0, vector1[2], vector1[1], vector1[0]);
        __m256 v2 = _mm256_set_ps(0, 0, 0, 0, 0, vector2[2], vector2[1], vector2[0]);

        __m256 shuffle1 = _mm256_permute_ps(v2, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 shuffle2 = _mm256_permute_ps(v1, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 mul1 = _mm256_mul_ps(shuffle1, shuffle2);

        shuffle1 = _mm256_permute_ps(v2, _MM_SHUFFLE(3, 1, 0, 2));
        shuffle2 = _mm256_permute_ps(v1, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 mul2 = _mm256_mul_ps(shuffle1, shuffle2);

        __m256 result = _mm256_sub_ps(mul2, mul1);

        return result;
    }

    static float vectorMagnitude(__m256 vec)
    {
        auto dot = _mm256_dp_ps(vec, vec, 0xFF);
        auto sqrt = _mm256_sqrt_ps(dot);
        return sqrt[0];
    }

    static float vectorMagnitude(const float *vector)
    {
        __m256 vec = _mm256_set_ps(0, 0, 0, 0, 0, vector[2], vector[1], vector[0]);
        auto dot = _mm256_dp_ps(vec, vec, 0xFF);
        auto sqrt = _mm256_sqrt_ps(dot);
        return sqrt[0];
    }

    static float mixedProduct(__m256 vector1, __m256 vector2, __m256 vector3)
    {

        __m256 shuffle1 = _mm256_permute_ps(vector3, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 shuffle2 = _mm256_permute_ps(vector2, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 mul1 = _mm256_mul_ps(shuffle1, shuffle2);

        shuffle1 = _mm256_permute_ps(vector3, _MM_SHUFFLE(3, 1, 0, 2));
        shuffle2 = _mm256_permute_ps(vector2, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 mul2 = _mm256_mul_ps(shuffle1, shuffle2);

        __m256 subres = _mm256_sub_ps(mul2, mul1);

        auto dotprod = _mm256_dp_ps(vector1, subres, 0xFF);

        return dotprod[0];
    }

    static float mixedProduct(const float *vector1, const float *vector2, const float *vector3)
    {
        __m256 v1 = _mm256_set_ps(0, 0, 0, 0, 0, vector1[2], vector1[1], vector1[0]);
        __m256 v2 = _mm256_set_ps(0, 0, 0, 0, 0, vector2[2], vector2[1], vector2[0]);
        __m256 v3 = _mm256_set_ps(0, 0, 0, 0, 0, vector3[2], vector3[1], vector3[0]);

        __m256 shuffle1 = _mm256_permute_ps(v3, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 shuffle2 = _mm256_permute_ps(v2, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 mul1 = _mm256_mul_ps(shuffle1, shuffle2);

        shuffle1 = _mm256_permute_ps(v3, _MM_SHUFFLE(3, 1, 0, 2));
        shuffle2 = _mm256_permute_ps(v2, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 mul2 = _mm256_mul_ps(shuffle1, shuffle2);

        __m256 subres = _mm256_sub_ps(mul2, mul1);

        auto dotprod = _mm256_dp_ps(v1, subres, 0xFF);

        return dotprod[0];
    }

    /** @brief Calculate track impact parameter corresponding to vertex. */
    static Double_t calculateImpactParameter(Vertex &vertex, Track *track)
    {
        __m256 trackPos = _mm256_set_ps(0, 0, 0, 0, 0, track->getZ(), track->getY(), track->getX());
        __m256 vertPos = _mm256_set_ps(0, 0, 0, 0, 0, vertex.getZ(), vertex.getY(), vertex.getX());
        __m256 trackDir = _mm256_set_ps(0, 0, 0, 0, 0, track->getTanZ(), track->getTanY(), track->getTanX());

        auto vertTrackDist = _mm256_sub_ps(trackPos, vertPos);
        auto distCrossDir = crossProduct(vertTrackDist, trackDir);

        auto distDirDot = _mm256_dp_ps(distCrossDir, distCrossDir, 0xFF);
        auto distDirMagn = _mm256_sqrt_ps(distDirDot);

        auto trackDirDot = _mm256_dp_ps(trackDir, trackDir, 0xFF);
        auto trackDirMagn = _mm256_sqrt_ps(trackDirDot);

        auto result = distDirMagn[0] / trackDirMagn[0];

        return result;
    }
};
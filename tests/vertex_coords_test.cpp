#include <gtest/gtest.h>
#include <cmath>
#include <optional>

#include "../src/data_types/DataObject.hpp"
#include "../src/data_types/Track.hpp"
#include "../src/data_types/Vertex.hpp"
#include "../src/vertex_search/VertexSearcher.hpp"
#include "../src/utility/CalculationAndAlgorithms.hpp"

TEST(VertexCoordsTest, CalculateVertexCoordsCorrectness)
{
    // https://mathter.pro/angem/5_5_2_skreschivayuschiesya_pryamye.html
    // https://mathter.pro/angem/5_5_3_kak_nayti_uravnenie_obschego_perpendikulyara.html

    const float VERTEX_TRACK_ACCUARACY = 2;

    const float varSandTExpected = -5.0 / 3.0;
    const float complanar = 4.0;

    Track t1(1, 2, -1, 0, 2, -3, -1);

    Track t2(1, -1, 0, 1, 1, -2, 0);

    __m256 coord1 = _mm256_set_ps(0, 0, 0, 0, 0, t1.getZ(), t1.getY(), t1.getX());
    __m256 coord2 = _mm256_set_ps(0, 0, 0, 0, 0, t2.getZ(), t2.getY(), t2.getX());
    __m256 dir1 = _mm256_set_ps(0, 0, 0, 0, 0, t1.getTanZ(), t1.getTanY(), t1.getTanX());
    __m256 dir2 = _mm256_set_ps(0, 0, 0, 0, 0, t2.getTanZ(), t2.getTanY(), t2.getTanX());

    auto pointDist = _mm256_sub_ps(coord2, coord1);
    auto mixed = std::abs(CalculationAndAlgorithms::mixedProduct(pointDist, dir1, dir2));
    EXPECT_EQ(mixed, complanar); // vectors are complanar
    auto dirDot = CalculationAndAlgorithms::crossProduct(dir1, dir2);
    auto dirDotMagn = CalculationAndAlgorithms::vectorMagnitude(dirDot);
    auto perpendicular = mixed / dirDotMagn;
    float perpFloored = floor(perpendicular * 100) / 100;

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
    auto varSfoored = floor(variableS * 100) / 100;
    auto varTfoored = floor(variableT * 100) / 100;

    float varSTExpFoored = floor(varSandTExpected * 100) / 100;
    EXPECT_EQ(varSfoored, varSTExpFoored);
    EXPECT_EQ(varTfoored, varSTExpFoored);

    __m256 h1 = _mm256_set_ps(0, 0, 0, 0, 0, t1.getTanZ() * variableT + t1.getZ(), t1.getTanY() * variableT + t1.getY(), t1.getTanX() * variableT + t1.getX());
    __m256 h2 = _mm256_set_ps(0, 0, 0, 0, 0, t2.getTanZ() * variableS + t2.getZ(), t2.getTanY() * variableS + t2.getY(), t2.getTanX() * variableS + t2.getX());
    __m256 hDelt = _mm256_sub_ps(h2, h1);
    auto hDeltMod = CalculationAndAlgorithms::vectorMagnitude(hDelt);
    float hDeltFloored = floor(hDeltMod * 100) / 100;

    EXPECT_EQ(hDeltFloored, perpFloored);

    Vertex vertex(floor(((t2.getTanX() * variableS + t2.getX() + t1.getTanX() * variableT + t1.getX()) / 2) * 100) / 100,
                  floor(((t2.getTanY() * variableS + t2.getY() + t1.getTanY() * variableT + t1.getY()) / 2) * 100) / 100,
                  floor(((t2.getTanZ() * variableS + t2.getZ() + t1.getTanZ() * variableT + t1.getZ()) / 2) * 100) / 100);

    __m256 vertCoord = _mm256_set_ps(0, 0, 0, 0, 0, vertex.getZ(), vertex.getY(), vertex.getX());
    __m256 h1DeltVert = _mm256_sub_ps(vertCoord, h1);
    __m256 h2DeltVert = _mm256_sub_ps(vertCoord, h2);
    auto h1VertDeltMod = CalculationAndAlgorithms::vectorMagnitude(h1DeltVert);
    float h1VertDeltModFloored = floor(h1VertDeltMod * 100) / 100;
    auto h2VertDeltMod = CalculationAndAlgorithms::vectorMagnitude(h1DeltVert);
    float h2VertDeltModFloored = floor(h2VertDeltMod * 100) / 100;
    EXPECT_EQ(h1VertDeltModFloored, h2VertDeltModFloored);

    // Now test the method itself, "VERTEX_TRACK_ACCUARACY" in method should be = 2
    VertexSearcher vs;
    auto vertexOneOpt = vs.calculateVertexCoordinates(t1, t2);
    ASSERT_EQ(vertexOneOpt.has_value(), true);
    if (vertexOneOpt.has_value())
    {
        auto vertexOne = vertexOneOpt.value();
        EXPECT_EQ(vertex.getX(), vertexOne.getX());
        EXPECT_EQ(vertex.getY(), vertexOne.getY());
        EXPECT_EQ(vertex.getZ(), vertexOne.getZ());
    }
}
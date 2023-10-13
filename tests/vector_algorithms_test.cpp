#include <gtest/gtest.h>

#include "../src/utility/CalculationAndAlgorithms.hpp"

TEST(VectorTest, MathematicalAlgorithmsCorrectness)
{
    __m256 m1vec = _mm256_set_ps(0, 0, 0, 0, 0, 3, -17, 1);
    __m256 m2vec = _mm256_set_ps(0, 0, 0, 0, 0, 1, -3, 2);
    __m256 m3vec = _mm256_set_ps(0, 0, 0, 0, 0, 0, -2, 1);

    float vec1[3] = {1, -17, 3};
    float vec2[3] = {2, -3, 1};
    float vec3[3] = {1, -2, 0};

    __m256 crossProductFloats = CalculationAndAlgorithms::crossProduct(vec2, vec3);
    EXPECT_EQ(crossProductFloats[0], 2);
    EXPECT_EQ(crossProductFloats[1], 1);
    EXPECT_EQ(crossProductFloats[2], -1);

    __m256 crossProductVectRegisters = CalculationAndAlgorithms::crossProduct(vec2, vec3);
    EXPECT_EQ(crossProductVectRegisters[0], 2);
    EXPECT_EQ(crossProductVectRegisters[1], 1);
    EXPECT_EQ(crossProductVectRegisters[2], -1);

    float mixedProductFloats = CalculationAndAlgorithms::mixedProduct(vec1, vec2, vec3);
    EXPECT_EQ(mixedProductFloats, -18);

    float mixedProductVectRegisters = CalculationAndAlgorithms::mixedProduct(m1vec, m2vec, m3vec);
    EXPECT_EQ(mixedProductVectRegisters, -18);

    float vectorMagnitude = CalculationAndAlgorithms::vectorMagnitude(vec1);
    float value1 = floor(vectorMagnitude * 100) / 100;
    float value2 = floor(std::sqrt(299) * 100) / 100;
    EXPECT_EQ(value1, value2);

    float vectorMagnitudeRegisters = CalculationAndAlgorithms::vectorMagnitude(m1vec);
    float value1m = floor(vectorMagnitudeRegisters * 100) / 100;
    float value2m = floor(std::sqrt(299) * 100) / 100;
    EXPECT_EQ(value1m, value2m);
}
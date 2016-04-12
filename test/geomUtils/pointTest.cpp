#include <gtest/gtest.h>

#include "geomUtils/point.h"

class PointTest : public ::testing::Test {
  protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(PointTest, size) {
  Point<float, 3> point;
  ASSERT_EQ(point[0], 0) << "Point initialized to zero by default";
  Point<float, 3> point1({1,2,3});
  ASSERT_EQ((Point<float, 3>(point1, point)[1]), 1.0f) << "Averaging not working";
}

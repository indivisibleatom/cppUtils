#include <gtest/gtest.h>

#include "geomUtils/point.h"

class PointTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(PointTest, size) {
  Point<float, 3> point1;
  ASSERT_EQ(point1[0], 0) << "Point initialized to zero by default";
  Point<float, 3> point2({1, 2, 3});
  ASSERT_EQ((Point<float, 3>(point1, point2)[1]), 1.0f)
      << "Averaging for two points failed";
  Point<float, 3> point3 = point2;
  ASSERT_EQ((Point<float, 3>({point1, point2, point3})[2]), 2.0f)
      << "Averaging for vector input failed";
}

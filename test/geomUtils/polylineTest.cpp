#include <gtest/gtest.h>

#include "geomUtils/polyline.h"

class PolylineTest : public ::testing::Test {
  protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(PolylineTest, size) {
  Polyline polyline;
}

#include <gtest/gtest.h>

// This is a simple sanity check to verify that
// GoogleTest is wired correctly and runs.
TEST(EngineSanity, BasicTruth) {
    EXPECT_TRUE(true);
}

// Example math test
TEST(EngineSanity, MathWorks) {
    EXPECT_EQ(2 + 2, 4);
}

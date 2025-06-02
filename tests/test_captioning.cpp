#include <gtest/gtest.h>
#include "Captioner.h"

TEST(Captioning, BasicVideo) {
    std::string videoPath = "../data/test_video.mp4";
    auto captions = Captioner::generateCaptions(videoPath, false);
    // TODO: Load expected JSON and compare
    ASSERT_FALSE(captions.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
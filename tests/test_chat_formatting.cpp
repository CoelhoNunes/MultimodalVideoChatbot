#include <gtest/gtest.h>
#include "Chatbot.h"

TEST(ChatFormatting, ContextPrompt) {
    std::vector<MatchResult> top3 = {
        {"video1.mp4", "00:00:10", "A dog runs across a field"},
        {"video2.mp4", "00:01:05", "Children playing soccer"},
        {"video3.mp4", "00:02:00", "A car drives by"}
    };
    std::string prompt = Chatbot::buildContextPrompt(top3);
    std::string expected =
      "Video: video1.mp4, Time: 00:00:10 — Caption: A dog runs across a field
"
      "Video: video2.mp4, Time: 00:01:05 — Caption: Children playing soccer
"
      "Video: video3.mp4, Time: 00:02:00 — Caption: A car drives by
"
      "Answer the user’s question based on the above captions.";
    EXPECT_EQ(prompt, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
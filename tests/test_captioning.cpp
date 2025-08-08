#include <gtest/gtest.h>
#include "../src/captioning/Captioner.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TEST(CaptionerTest, GenerateCaptions) {
    Captioner captioner;
    
    // Test with a mock video file (you'll need to provide an actual test video)
    std::string testVideoPath = "../data/test_video.mp4";
    
    // Check if test video exists
    std::ifstream file(testVideoPath);
    if (!file.good()) {
        std::cout << "Test video not found, skipping test" << std::endl;
        GTEST_SKIP();
        return;
    }
    
    std::vector<CaptionResult> captions = captioner.generateCaptions(testVideoPath);
    
    // Basic validation
    EXPECT_FALSE(captions.empty()) << "Should generate at least one caption";
    
    for (const auto& caption : captions) {
        EXPECT_GE(caption.timestamp, 0.0) << "Timestamp should be non-negative";
        EXPECT_FALSE(caption.text.empty()) << "Caption text should not be empty";
        EXPECT_GE(caption.confidence, 0.0) << "Confidence should be non-negative";
        EXPECT_LE(caption.confidence, 1.0) << "Confidence should be at most 1.0";
    }
    
    // Check that timestamps are in ascending order
    for (size_t i = 1; i < captions.size(); ++i) {
        EXPECT_GE(captions[i].timestamp, captions[i-1].timestamp) 
            << "Captions should be in chronological order";
    }
}

TEST(CaptionerTest, MockImplementation) {
    Captioner captioner;
    
    // Test with a non-existent file to trigger mock implementation
    std::string nonExistentPath = "non_existent_video.mp4";
    
    std::vector<CaptionResult> captions = captioner.generateCaptions(nonExistentPath);
    
    // Mock implementation should still provide some output
    EXPECT_FALSE(captions.empty()) << "Mock implementation should provide captions";
    
    for (const auto& caption : captions) {
        EXPECT_GE(caption.timestamp, 0.0) << "Timestamp should be non-negative";
        EXPECT_FALSE(caption.text.empty()) << "Caption text should not be empty";
        EXPECT_GE(caption.confidence, 0.0) << "Confidence should be non-negative";
        EXPECT_LE(caption.confidence, 1.0) << "Confidence should be at most 1.0";
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
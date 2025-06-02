#pragma once

#include <string>
#include <vector>

// Represents a generated caption with timestamp
struct CaptionResult {
    std::string timestamp;
    std::string text;
};

// Captioner handles video frame extraction and caption generation
class Captioner {
public:
    // Generate captions for a given video file path
    // use_gpu: whether to use GPU for ONNX Runtime
    static std::vector<CaptionResult> generateCaptions(const std::string &videoPath, bool use_gpu);
};
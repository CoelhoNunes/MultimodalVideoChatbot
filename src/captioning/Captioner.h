#pragma once

#include <string>
#include <vector>
#include <memory>

struct CaptionResult {
    double timestamp;
    std::string text;
    float confidence;
};

class Captioner {
public:
    Captioner();
    ~Captioner();
    
    std::vector<CaptionResult> generateCaptions(const std::string& videoPath);

private:
    std::string runInference(const std::vector<float>& inputTensor);
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
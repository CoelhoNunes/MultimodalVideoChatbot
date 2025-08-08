#pragma once

#include <string>
#include <vector>
#include <memory>

class Embedder {
public:
    Embedder();
    ~Embedder();
    
    std::vector<float> embedText(const std::string& text);

private:
    std::vector<int64_t> tokenize(const std::string& text);
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
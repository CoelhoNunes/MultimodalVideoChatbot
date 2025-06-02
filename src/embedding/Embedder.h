#pragma once

#include <string>
#include <vector>

// Represents an embedding vector
using Embedding = std::vector<float>;

class Embedder {
public:
    // Initialize the text encoder (ONNX) and tokenizer
    static void initialize(const std::string &modelPath, const std::string &vocabPath, const std::string &mergePath);
    // Embed a single caption string
    static Embedding embedCaption(const std::string &caption);
};
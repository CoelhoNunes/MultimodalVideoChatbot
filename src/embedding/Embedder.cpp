#include "Embedder.h"
#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

struct Embedder::Impl {
    Ort::Env env;
    Ort::Session session{nullptr};
    std::unique_ptr<Ort::SessionOptions> sessionOptions;
    std::vector<std::string> vocabulary;
    
    ~Impl() = default;
};

Embedder::Embedder() : pImpl(std::make_unique<Impl>()) {
    // Initialize ONNX Runtime
    pImpl->env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "text_embedder");
    pImpl->sessionOptions = std::make_unique<Ort::SessionOptions>();
    pImpl->sessionOptions->SetIntraOpNumThreads(1);
    pImpl->sessionOptions->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    
    // Load model (you'll need to provide the actual model path)
    try {
        pImpl->session = Ort::Session(pImpl->env, "models/text_encoder.onnx", *pImpl->sessionOptions);
    } catch (const Ort::Exception& e) {
        std::cerr << "Failed to load ONNX model: " << e.what() << std::endl;
        // For demo purposes, we'll create a mock implementation
    }
    
    // Initialize vocabulary (mock for demo)
    pImpl->vocabulary = {"[PAD]", "[UNK]", "[CLS]", "[SEP]", "the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by"};
}

Embedder::~Embedder() = default;

std::vector<float> Embedder::embedText(const std::string& text) {
    if (!pImpl->session) {
        // Mock implementation for demo - return a fixed-size embedding
        std::vector<float> mockEmbedding(768, 0.0f);
        for (size_t i = 0; i < std::min(text.length(), size_t(768)); ++i) {
            mockEmbedding[i] = static_cast<float>(text[i]) / 255.0f;
        }
        return mockEmbedding;
    }
    
    try {
        // Tokenize text
        std::vector<int64_t> tokens = tokenize(text);
        
        // Prepare input tensor
        std::vector<int64_t> inputShape = {1, static_cast<int64_t>(tokens.size())};
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        
        Ort::Value inputTensorOrt = Ort::Value::CreateTensor<int64_t>(
            memory_info, tokens.data(), tokens.size(),
            inputShape.data(), inputShape.size()
        );
        
        // Run inference
        auto outputTensor = pImpl->session.Run(
            Ort::RunOptions{nullptr}, 
            &pImpl->session.GetInputName(0, Ort::AllocatorWithDefaultOptions()), 
            &inputTensorOrt, 1, 
            &pImpl->session.GetOutputName(0, Ort::AllocatorWithDefaultOptions()), 1
        );
        
        // Extract embedding from output tensor
        float* outputData = outputTensor[0].GetTensorMutableData<float>();
        size_t outputSize = outputTensor[0].GetTensorTypeAndShapeInfo().GetElementCount();
        
        std::vector<float> embedding(outputData, outputData + outputSize);
        return embedding;
        
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX inference failed: " << e.what() << std::endl;
        // Return mock embedding on error
        std::vector<float> mockEmbedding(768, 0.0f);
        for (size_t i = 0; i < std::min(text.length(), size_t(768)); ++i) {
            mockEmbedding[i] = static_cast<float>(text[i]) / 255.0f;
        }
        return mockEmbedding;
    }
}

std::vector<int64_t> Embedder::tokenize(const std::string& text) {
    // Simple tokenization for demo purposes
    // In a real implementation, you'd use a proper tokenizer like BPE or WordPiece
    
    std::vector<int64_t> tokens;
    tokens.push_back(2); // [CLS] token
    
    // Simple word-based tokenization
    std::string lowerText = text;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
    
    std::string word;
    for (char c : lowerText) {
        if (std::isalpha(c) || std::isdigit(c)) {
            word += c;
        } else if (!word.empty()) {
            // Look up word in vocabulary
            auto it = std::find(pImpl->vocabulary.begin(), pImpl->vocabulary.end(), word);
            if (it != pImpl->vocabulary.end()) {
                tokens.push_back(std::distance(pImpl->vocabulary.begin(), it));
            } else {
                tokens.push_back(1); // [UNK] token
            }
            word.clear();
        }
    }
    
    // Add last word if any
    if (!word.empty()) {
        auto it = std::find(pImpl->vocabulary.begin(), pImpl->vocabulary.end(), word);
        if (it != pImpl->vocabulary.end()) {
            tokens.push_back(std::distance(pImpl->vocabulary.begin(), it));
        } else {
            tokens.push_back(1); // [UNK] token
        }
    }
    
    tokens.push_back(3); // [SEP] token
    
    // Pad to maximum length if needed
    const int maxLength = 512;
    while (tokens.size() < maxLength) {
        tokens.push_back(0); // [PAD] token
    }
    
    return tokens;
}
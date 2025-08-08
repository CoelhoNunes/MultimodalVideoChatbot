#pragma once

#include <string>
#include <vector>
#include <memory>

struct MatchResult {
    std::string video;
    double timestamp;
    std::string caption;
    float similarity;
};

struct ChatResponse {
    std::string text;
    std::vector<MatchResult> matches;
    std::string detectedLanguage;
};

// Forward declaration
struct CaptionResult;

class Chatbot {
public:
    Chatbot();
    ~Chatbot();
    
    ChatResponse processQuery(const std::string& query, const std::vector<CaptionResult>& captions);

private:
    std::string detectLanguage(const std::string& text);
    std::string translateToEnglish(const std::string& text, const std::string& sourceLang);
    std::string translateFromEnglish(const std::string& text, const std::string& targetLang);
    std::vector<MatchResult> searchSimilarCaptions(const std::string& query, const std::vector<CaptionResult>& captions, int topK = 5);
    std::string generateResponse(const std::string& query, const std::vector<MatchResult>& matches);
    std::vector<int64_t> tokenize(const std::string& text);
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
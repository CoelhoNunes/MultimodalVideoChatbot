#pragma once

#include <string>
#include <vector>
#include "Captioner.h"

// Represents a match result from FAISS
struct MatchResult {
    std::string video;
    std::string timestamp;
    std::string caption;
};

class Chatbot {
public:
    // Build context prompt from top-k matches
    static std::string buildContextPrompt(const std::vector<MatchResult> &matches);
    // Process a user query: language detection, translation, embedding, retrieval, conversational inference, reverse translation
    static std::pair<std::string, std::vector<MatchResult>> processQuery(const std::string &text);
};
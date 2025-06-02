#include "Chatbot.h"
// TODO: Include fastText, ONNX Runtime, libcurl for translation, FAISS, and conversational model headers

std::string Chatbot::buildContextPrompt(const std::vector<MatchResult> &matches) {
    std::string context;
    for (const auto &m : matches) {
        context += "Video: " + m.video + ", Time: " + m.timestamp + " — Caption: " + m.caption + "\n";
    }
    context += "Answer the user’s question based on the above captions.";
    return context;
}

std::pair<std::string, std::vector<MatchResult>> Chatbot::processQuery(const std::string &text) {
    // TODO: Detect language via fastText
    // TODO: If non-English, translate to English via Hugging Face API
    // TODO: Embed query, run FAISS search, get top-k MatchResult
    // TODO: Build context prompt, run conversational ONNX inference streaming tokens
    // TODO: Reverse translate response into user language
    // Return pair of final response string and match results vector
    return { "", {} };
}
#include "Captioner.h"
// TODO: Include FFmpeg and ONNX Runtime headers

std::vector<CaptionResult> Captioner::generateCaptions(const std::string &videoPath, bool use_gpu) {
    // TODO: Implement frame extraction via FFmpeg C API
    // TODO: Preprocess frames, batch into tensors, run ONNX Runtime for video–text model
    // TODO: Generate captions every N seconds, return a vector of CaptionResult
    return {};
}
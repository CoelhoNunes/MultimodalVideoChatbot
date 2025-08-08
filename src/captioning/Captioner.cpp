#include "Captioner.h"
#include <onnxruntime_cxx_api.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

struct Captioner::Impl {
    Ort::Env env;
    Ort::Session session{nullptr};
    std::unique_ptr<Ort::SessionOptions> sessionOptions;
    SwsContext* swsCtx = nullptr;
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* codecCtx = nullptr;
    int videoStream = -1;
    
    ~Impl() {
        if (swsCtx) sws_freeContext(swsCtx);
        if (codecCtx) avcodec_free_context(&codecCtx);
        if (formatCtx) avformat_close_input(&formatCtx);
    }
};

Captioner::Captioner() : pImpl(std::make_unique<Impl>()) {
    // Initialize ONNX Runtime
    pImpl->env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "video_captioner");
    pImpl->sessionOptions = std::make_unique<Ort::SessionOptions>();
    pImpl->sessionOptions->SetIntraOpNumThreads(1);
    pImpl->sessionOptions->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    
    // Load model (you'll need to provide the actual model path)
    try {
        pImpl->session = Ort::Session(pImpl->env, "models/video_captioning.onnx", *pImpl->sessionOptions);
    } catch (const Ort::Exception& e) {
        std::cerr << "Failed to load ONNX model: " << e.what() << std::endl;
        // For demo purposes, we'll create a mock implementation
    }
}

Captioner::~Captioner() = default;

std::vector<CaptionResult> Captioner::generateCaptions(const std::string& videoPath) {
    std::vector<CaptionResult> results;
    
    // Open video file
    if (avformat_open_input(&pImpl->formatCtx, videoPath.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open video file: " << videoPath << std::endl;
        return results;
    }
    
    // Find stream info
    if (avformat_find_stream_info(pImpl->formatCtx, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        return results;
    }
    
    // Find video stream
    for (unsigned int i = 0; i < pImpl->formatCtx->nb_streams; i++) {
        if (pImpl->formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            pImpl->videoStream = i;
            break;
        }
    }
    
    if (pImpl->videoStream == -1) {
        std::cerr << "Could not find video stream" << std::endl;
        return results;
    }
    
    // Get codec context
    const AVCodec* codec = avcodec_find_decoder(pImpl->formatCtx->streams[pImpl->videoStream]->codecpar->codec_id);
    if (!codec) {
        std::cerr << "Unsupported codec" << std::endl;
        return results;
    }
    
    pImpl->codecCtx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(pImpl->codecCtx, pImpl->formatCtx->streams[pImpl->videoStream]->codecpar) < 0) {
        std::cerr << "Could not copy codec params" << std::endl;
        return results;
    }
    
    if (avcodec_open2(pImpl->codecCtx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        return results;
    }
    
    // Initialize software scaler
    pImpl->swsCtx = sws_getContext(
        pImpl->codecCtx->width, pImpl->codecCtx->height, pImpl->codecCtx->pix_fmt,
        224, 224, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (!pImpl->swsCtx) {
        std::cerr << "Could not initialize software scaler" << std::endl;
        return results;
    }
    
    // Read frames and generate captions
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, 224, 224, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes);
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, 224, 224, 1);
    
    double frameRate = av_q2d(pImpl->formatCtx->streams[pImpl->videoStream]->r_frame_rate);
    int frameInterval = static_cast<int>(frameRate * 2.0); // Generate caption every 2 seconds
    int frameCount = 0;
    
    while (av_read_frame(pImpl->formatCtx, packet) >= 0) {
        if (packet->stream_index == pImpl->videoStream) {
            int response = avcodec_send_packet(pImpl->codecCtx, packet);
            if (response < 0) {
                av_packet_unref(packet);
                continue;
            }
            
            while (response >= 0) {
                response = avcodec_receive_frame(pImpl->codecCtx, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    break;
                }
                
                frameCount++;
                
                // Generate caption every N frames
                if (frameCount % frameInterval == 0) {
                    // Scale frame to 224x224 RGB
                    sws_scale(pImpl->swsCtx, frame->data, frame->linesize, 0, 
                             pImpl->codecCtx->height, rgbFrame->data, rgbFrame->linesize);
                    
                    // Convert to tensor and run inference
                    std::vector<float> inputTensor(3 * 224 * 224);
                    for (int i = 0; i < 224 * 224; i++) {
                        inputTensor[i] = rgbFrame->data[0][i * 3] / 255.0f;     // R
                        inputTensor[i + 224 * 224] = rgbFrame->data[0][i * 3 + 1] / 255.0f; // G
                        inputTensor[i + 2 * 224 * 224] = rgbFrame->data[0][i * 3 + 2] / 255.0f; // B
                    }
                    
                    // Run ONNX inference
                    std::string caption = runInference(inputTensor);
                    
                    // Calculate timestamp
                    double timestamp = static_cast<double>(frameCount) / frameRate;
                    
                    CaptionResult result;
                    result.timestamp = timestamp;
                    result.text = caption;
                    result.confidence = 0.85f; // Mock confidence
                    
                    results.push_back(result);
                }
            }
        }
        av_packet_unref(packet);
    }
    
    // Cleanup
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    av_packet_free(&packet);
    av_free(buffer);
    
    return results;
}

std::string Captioner::runInference(const std::vector<float>& inputTensor) {
    if (!pImpl->session) {
        // Mock implementation for demo
        return "A person is shown in the video";
    }
    
    try {
        // Prepare input tensor
        std::vector<int64_t> inputShape = {1, 3, 224, 224};
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        
        Ort::Value inputTensorOrt = Ort::Value::CreateTensor<float>(
            memory_info, const_cast<float*>(inputTensor.data()), inputTensor.size(),
            inputShape.data(), inputShape.size()
        );
        
        // Run inference
        auto outputTensor = pImpl->session.Run(
            Ort::RunOptions{nullptr}, 
            &pImpl->session.GetInputName(0, Ort::AllocatorWithDefaultOptions()), 
            &inputTensorOrt, 1, 
            &pImpl->session.GetOutputName(0, Ort::AllocatorWithDefaultOptions()), 1
        );
        
        // Process output (this would depend on your model's output format)
        // For now, return a mock caption
        return "A person is shown in the video";
        
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX inference failed: " << e.what() << std::endl;
        return "Error processing video frame";
    }
}
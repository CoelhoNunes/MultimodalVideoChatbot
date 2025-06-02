# 🎥🤖 Multimodal Video Captioning & Multilingual Chatbot

> A high-performance C++ application that generates time‐aligned captions for videos, indexes them for semantic search, and exposes a real-time multilingual chatbot interface for querying your entire video archive. Built with FFmpeg, ONNX Runtime, FAISS, fastText, Boost.Asio, and Qt—plus a shareable Gradio demo.

---

## 🚀 Features

- **Frame Extraction & Captioning**  
  - Extracts frames from videos at configurable FPS using **FFmpeg C API**.  
  - Runs a Hugging Face “VideoMAE encoder + BART decoder” model (exported to **ONNX Runtime**) to generate captions every *N* seconds.  
  - Preprocesses and batches frames efficiently, with optional **GPU (CUDA)** acceleration.

- **Caption Embedding & FAISS Indexing**  
  - Embeds each generated caption using a Hugging Face **SentenceTransformer** (exported to ONNX).  
  - Builds a FAISS index (HNSW + IVF+PQ quantization) for fast, approximate nearest‐neighbor search over thousands of captions.  

- **Multilingual Chatbot‐Driven Search**  
  - Detects user language with **fastText** (176-language model).  
  - If non‐English, translates queries to English via **Hugging Face Translation API** (libcurl).  
  - Retrieves top‐K matching captions (video + timestamp) via FAISS.  
  - Constructs a context prompt and performs streaming conversational inference using a Hugging Face **DialoGPT/GPT-Neo** ONNX model.  
  - Translates the chatbot’s English response back into the user’s language.  
  - Provides a rule‐based fallback message when APIs are unavailable.  
  - Enforces API rate‐limiting (max 5 requests/sec) to stay within quotas.

- **Front‐End Demos**  
  - **Qt Desktop GUI**: Two‐tab interface (“Video Captioning” & “Chat & Search”) with:  
    - Directory selection, file‐by‐file progress bar, and a clickable timeline of captions.  
    - Embedded QVideoWidget video player that seeks to any caption timestamp.  
    - Chat input area (any language) with streaming responses and a list of clickable “video @ timestamp” matches.  
  - **Gradio Web Demo**: Rapid, shareable interface (Python “glue” → C++ backend) featuring:  
    1. Video uploader → “Generate Captions” → timeline viewer in HTML.  
    2. Multilingual chat box → “Ask” → chatbot response + clickable video‐segment links.  
    - Deployable on Hugging Face Spaces with a single `app.py`.

- **Testing & Evaluation**  
  - **Google Test** integration:  
    - Verify caption generation against a short test video.  
    - Confirm FAISS retrieval correctness on synthetic embeddings.  
    - Validate context‐prompt formatting.  
  - **Benchmark Scripts**:  
    - `benchmark_captions.sh`: Compare GPU vs. CPU caption‐generation performance (avg FPS, total time).  
    - `benchmark_search.sh`: Measure end‐to‐end chat query latency (language detection, translation, FAISS, inference, reverse translation).  
  - **Caption Quality Metrics**: Python script to compute **BLEU** & **CIDEr** against a held‐out ground truth.  
  - **Multilingual Correctness**: JSON‐driven tests for equivalent queries in English, Spanish, and Mandarin (precision@K).

- **Docker & CI**  
  - **Dockerfile.server**: Ubuntu 22.04 image with FFmpeg, ONNX Runtime, FAISS, fastText—builds and runs the WebSocket server.  
  - **Dockerfile.gui**: Ubuntu 22.04 image configured to build and run the Qt GUI (X11 forwarding or VNC/XPRA).  
  - **GitHub Actions CI**:  
    - Builds (CPU mode, tests ON) and runs `ctest`.  
    - Builds/pushes both server and GUI Docker images.  

---

## 🔧 Prerequisites

1. **System Dependencies** (Ubuntu 22.04 example)  
   ```bash
   sudo apt-get update
   sudo apt-get install -y \
       build-essential cmake git wget unzip \
       ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev \
       libboost-all-dev qtbase5-dev qttools5-dev libqt5multimedia5-plugins libqt5multimediawidgets5 \
       python3 python3-pip curl

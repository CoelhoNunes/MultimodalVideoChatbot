# Build Instructions

## Linux (Ubuntu 22.04)

1. Install system dependencies:
   ```
   sudo apt-get update
   sudo apt-get install -y \
       build-essential \
       cmake \
       git \
       ffmpeg \
       libavcodec-dev \
       libavformat-dev \
       libavutil-dev \
       libswscale-dev \
       libboost-all-dev \
       qtbase5-dev \
       qttools5-dev \
       libqt5multimedia5-plugins \
       libqt5multimediawidgets5 \
       python3 \
       python3-pip \
       curl
   ```

2. Install ONNX Runtime (CPU):
   ```
   pip3 install onnxruntime
   ```

3. Install FAISS (CPU):
   ```
   sudo apt-get update
   sudo apt-get install -y libfaiss-dev
   ```

4. Install fastText:
   ```
   git clone https://github.com/facebookresearch/fastText.git /fasttext
   cd /fasttext
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   sudo cp ../fasttext ../libfasttext.so /usr/local/lib/
   sudo cp ../*.h /usr/local/include/
   sudo ldconfig
   ```

5. Clone this repository and build:
   ```
   git clone https://github.com/YourUsername/MultimodalVideoChatbot.git
   cd MultimodalVideoChatbot
   mkdir build && cd build
   cmake -DUSE_CUDA=OFF -DBUILD_GUI=OFF -DBUILD_TESTS=ON ..
   make -j$(nproc)
   ctest --output-on-failure
   ```

## Windows / macOS
- Follow similar steps using corresponding package managers and install paths.
- Ensure FFmpeg, ONNX Runtime, FAISS, and fastText libraries are installed and reachable.
#!/bin/bash

# Build script for Multimodal Video Chatbot

set -e

echo "Building Multimodal Video Chatbot..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GUI=ON \
    -DBUILD_TESTS=ON \
    -DUSE_CUDA=OFF

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  - ./build/captioner          # Video captioning tool"
echo "  - ./build/websocket_server   # WebSocket server"
echo "  - ./build/VideoChatbot       # GUI application"
echo ""
echo "To run the GUI application:"
echo "  1. Start the WebSocket server: ./build/websocket_server"
echo "  2. In another terminal, run: ./build/VideoChatbot"
echo ""
echo "To run the Gradio demo:"
echo "  1. Start the WebSocket server: ./build/websocket_server"
echo "  2. In another terminal, run: cd gradio_demo && python app.py"



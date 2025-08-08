#!/bin/bash

# Comprehensive setup script for Multimodal Video Chatbot
# This script installs all dependencies and sets up the project

set -e

echo "🚀 Setting up Multimodal Video Chatbot..."
echo "=========================================="

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    OS="windows"
else
    echo "❌ Unsupported OS: $OSTYPE"
    exit 1
fi

echo "📋 Detected OS: $OS"

# Function to install system dependencies
install_system_deps() {
    echo ""
    echo "📦 Installing system dependencies..."
    
    if [[ "$OS" == "linux" ]]; then
        # Check if we're on Ubuntu/Debian
        if command -v apt-get &> /dev/null; then
            echo "Installing dependencies for Ubuntu/Debian..."
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                git \
                wget \
                curl \
                unzip \
                pkg-config \
                libssl-dev \
                libffi-dev \
                python3-dev \
                python3-pip \
                python3-venv \
                qt5-default \
                qttools5-dev-tools \
                libqt5multimedia5-plugins \
                libqt5multimediawidgets5 \
                libavformat-dev \
                libavcodec-dev \
                libavutil-dev \
                libswscale-dev \
                libswresample-dev \
                libgtest-dev \
                nlohmann-json3-dev \
                libboost-all-dev \
                libfaiss-dev \
                libfasttext-dev \
                libonnxruntime-dev
        elif command -v yum &> /dev/null; then
            echo "Installing dependencies for CentOS/RHEL..."
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y \
                cmake \
                git \
                qt5-qtbase-devel \
                qt5-qtmultimedia-devel \
                ffmpeg-devel \
                gtest-devel \
                boost-devel \
                python3-devel \
                python3-pip
        else
            echo "❌ Unsupported Linux distribution. Please install dependencies manually."
            exit 1
        fi
    elif [[ "$OS" == "macos" ]]; then
        echo "Installing dependencies for macOS..."
        if ! command -v brew &> /dev/null; then
            echo "Installing Homebrew..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        fi
        
        brew install \
            cmake \
            qt5 \
            ffmpeg \
            onnxruntime \
            faiss \
            fasttext \
            googletest \
            nlohmann-json \
            boost
    else
        echo "❌ Windows setup not supported in this script. Please use WSL or install manually."
        exit 1
    fi
}

# Function to install Python dependencies
install_python_deps() {
    echo ""
    echo "🐍 Installing Python dependencies..."
    
    # Create virtual environment if it doesn't exist
    if [[ ! -d "venv" ]]; then
        echo "Creating Python virtual environment..."
        python3 -m venv venv
    fi
    
    # Activate virtual environment
    source venv/bin/activate
    
    # Upgrade pip
    pip install --upgrade pip
    
    # Install Python dependencies
    echo "Installing Python packages..."
    pip install -r gradio_demo/requirements.txt
    
    # Install additional packages that might be needed
    pip install \
        numpy \
        requests \
        gradio \
        pathlib2 \
        pytest \
        pytest-cov \
        black \
        flake8
}

# Function to create models directory and download sample models
setup_models() {
    echo ""
    echo "🤖 Setting up model directory..."
    
    mkdir -p models
    
    # Create a simple ONNX model for testing (if none exists)
    if [[ ! -f "models/video_captioning.onnx" ]]; then
        echo "Creating mock ONNX models for testing..."
        python3 -c "
import numpy as np
import onnx
from onnx import helper, numpy_helper

# Create a simple mock model
def create_mock_model():
    # Input
    input_tensor = helper.make_tensor_value_info(
        'input', onnx.TensorProto.FLOAT, [1, 3, 224, 224]
    )
    
    # Output
    output_tensor = helper.make_tensor_value_info(
        'output', onnx.TensorProto.FLOAT, [1, 512]
    )
    
    # Create a simple graph
    graph = helper.make_graph(
        [helper.make_node('Identity', ['input'], ['output'])],
        'mock_model',
        [input_tensor],
        [output_tensor]
    )
    
    # Create model
    model = helper.make_model(graph)
    return model

# Save models
model = create_mock_model()
with open('models/video_captioning.onnx', 'wb') as f:
    f.write(model.SerializeToString())

with open('models/text_encoder.onnx', 'wb') as f:
    f.write(model.SerializeToString())

with open('models/conversational_model.onnx', 'wb') as f:
    f.write(model.SerializeToString())

print('Mock ONNX models created successfully!')
"
    fi
}

# Function to create test data
setup_test_data() {
    echo ""
    echo "📁 Setting up test data..."
    
    mkdir -p data
    
    # Create a simple test video if none exists
    if [[ ! -f "data/test_video.mp4" ]]; then
        echo "Creating test video..."
        # Use ffmpeg to create a simple test video
        ffmpeg -f lavfi -i testsrc=duration=10:size=320x240:rate=1 \
               -f lavfi -i sine=frequency=1000:duration=10 \
               -c:v libx264 -c:a aac \
               data/test_video.mp4 -y 2>/dev/null || echo "Could not create test video (ffmpeg not available)"
    fi
    
    # Create test JSON files
    cat > data/ground_truth_captions.json << 'EOF'
[
  {"timestamp": 0.0, "text": "A person is shown in the video", "confidence": 0.85},
  {"timestamp": 2.0, "text": "The person is speaking", "confidence": 0.92},
  {"timestamp": 4.0, "text": "The scene continues", "confidence": 0.78},
  {"timestamp": 6.0, "text": "More activity is shown", "confidence": 0.88},
  {"timestamp": 8.0, "text": "The video concludes", "confidence": 0.91}
]
EOF

    cat > data/multilingual_tests.json << 'EOF'
{
  "queries": [
    {"text": "What is happening in the video?", "language": "en"},
    {"text": "¿Qué está pasando en el video?", "language": "es"},
    {"text": "Que se passe-t-il dans la vidéo?", "language": "fr"},
    {"text": "Was passiert im Video?", "language": "de"}
  ]
}
EOF
}

# Function to build the project
build_project() {
    echo ""
    echo "🔨 Building the project..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    echo "Configuring with CMake..."
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_GUI=ON \
        -DBUILD_TESTS=ON \
        -DUSE_CUDA=OFF \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    
    # Build
    echo "Building..."
    make -j$(nproc 2>/dev/null || echo 4)
    
    cd ..
}

# Function to run tests
run_tests() {
    echo ""
    echo "🧪 Running tests..."
    
    # Activate virtual environment
    source venv/bin/activate
    
    # Run Python tests
    echo "Running Python setup tests..."
    python3 scripts/test_setup.py
    
    # Run C++ tests if available
    if [[ -f "build/test_captioning" ]]; then
        echo "Running C++ tests..."
        cd build && make test && cd ..
    fi
}

# Function to create environment file
create_env_file() {
    echo ""
    echo "📝 Creating environment configuration..."
    
    cat > .env << 'EOF'
# Multimodal Video Chatbot Environment Configuration

# Optional: Hugging Face API Token for enhanced translation
# Get your free token from: https://huggingface.co/settings/tokens
# HUGGINGFACE_API_TOKEN=your_token_here

# Server configuration
WEBSOCKET_SERVER_PORT=8080
GRADIO_SERVER_PORT=7860

# Model paths (optional)
VIDEO_CAPTIONING_MODEL=models/video_captioning.onnx
TEXT_ENCODER_MODEL=models/text_encoder.onnx
CONVERSATIONAL_MODEL=models/conversational_model.onnx

# Logging
LOG_LEVEL=INFO
EOF

    echo "Created .env file with default configuration"
}

# Function to create run scripts
create_run_scripts() {
    echo ""
    echo "📜 Creating run scripts..."
    
    # Create run server script
    cat > run_server.sh << 'EOF'
#!/bin/bash
echo "Starting WebSocket server..."
./build/websocket_server
EOF

    # Create run GUI script
    cat > run_gui.sh << 'EOF'
#!/bin/bash
echo "Starting GUI application..."
./build/VideoChatbot
EOF

    # Create run web demo script
    cat > run_web_demo.sh << 'EOF'
#!/bin/bash
echo "Starting Gradio web demo..."
cd gradio_demo
source ../venv/bin/activate
python app.py
EOF

    # Make scripts executable
    chmod +x run_server.sh run_gui.sh run_web_demo.sh
}

# Main setup function
main() {
    echo "Starting setup process..."
    
    # Install system dependencies
    install_system_deps
    
    # Install Python dependencies
    install_python_deps
    
    # Setup models
    setup_models
    
    # Setup test data
    setup_test_data
    
    # Build project
    build_project
    
    # Create environment file
    create_env_file
    
    # Create run scripts
    create_run_scripts
    
    # Run tests
    run_tests
    
    echo ""
    echo "🎉 Setup completed successfully!"
    echo ""
    echo "📋 Next steps:"
    echo "1. Start the WebSocket server: ./run_server.sh"
    echo "2. In another terminal, run the GUI: ./run_gui.sh"
    echo "3. Or run the web demo: ./run_web_demo.sh"
    echo ""
    echo "📖 For more information, see README.md"
    echo ""
    echo "🔧 Optional: Set your Hugging Face API token in .env file for enhanced features"
}

# Run main function
main "$@"



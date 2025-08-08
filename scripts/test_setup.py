#!/usr/bin/env python3
"""
Test script to verify the Multimodal Video Chatbot setup
"""

import os
import sys
import subprocess
import json
import requests
from pathlib import Path

def check_python_dependencies():
    """Check if Python dependencies are installed"""
    print("Checking Python dependencies...")
    
    required_packages = [
        'gradio',
        'requests', 
        'numpy',
        'pathlib2'
    ]
    
    missing = []
    for package in required_packages:
        try:
            __import__(package)
            print(f"  ✓ {package}")
        except ImportError:
            missing.append(package)
            print(f"  ✗ {package} (missing)")
    
    if missing:
        print(f"\nMissing packages: {', '.join(missing)}")
        print("Install with: pip install -r gradio_demo/requirements.txt")
        return False
    
    print("  ✓ All Python dependencies found")
    return True

def check_executables():
    """Check if C++ executables are built"""
    print("\nChecking C++ executables...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("  ✗ Build directory not found")
        print("  Run: ./scripts/build.sh")
        return False
    
    executables = [
        "captioner",
        "websocket_server", 
        "VideoChatbot"
    ]
    
    missing = []
    for exe in executables:
        exe_path = build_dir / exe
        if exe_path.exists():
            print(f"  ✓ {exe}")
        else:
            missing.append(exe)
            print(f"  ✗ {exe} (missing)")
    
    if missing:
        print(f"\nMissing executables: {', '.join(missing)}")
        print("Build the project with: ./scripts/build.sh")
        return False
    
    print("  ✓ All executables found")
    return True

def check_models():
    """Check if model files exist"""
    print("\nChecking model files...")
    
    models_dir = Path("models")
    if not models_dir.exists():
        print("  ⚠ models/ directory not found (optional)")
        return True
    
    model_files = [
        "video_captioning.onnx",
        "text_encoder.onnx",
        "conversational_model.onnx"
    ]
    
    found = []
    for model in model_files:
        model_path = models_dir / model
        if model_path.exists():
            found.append(model)
            print(f"  ✓ {model}")
        else:
            print(f"  ⚠ {model} (optional)")
    
    if found:
        print(f"  ✓ Found {len(found)} model files")
    else:
        print("  ⚠ No model files found (will use mock implementations)")
    
    return True

def test_websocket_server():
    """Test if WebSocket server can start"""
    print("\nTesting WebSocket server...")
    
    try:
        # Try to start server in background
        process = subprocess.Popen(
            ["./build/websocket_server"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # Give it a moment to start
        import time
        time.sleep(2)
        
        # Check if process is still running
        if process.poll() is None:
            print("  ✓ WebSocket server started successfully")
            process.terminate()
            process.wait()
            return True
        else:
            stdout, stderr = process.communicate()
            print(f"  ✗ WebSocket server failed to start")
            print(f"    Error: {stderr}")
            return False
            
    except Exception as e:
        print(f"  ✗ Error testing WebSocket server: {e}")
        return False

def test_gradio_demo():
    """Test if Gradio demo can be imported"""
    print("\nTesting Gradio demo...")
    
    try:
        # Add gradio_demo to path
        sys.path.insert(0, str(Path("gradio_demo")))
        
        # Try to import the backend bridge
        from backend_bridge import generate_captions, query_chatbot
        
        print("  ✓ Gradio demo imports successfully")
        return True
        
    except Exception as e:
        print(f"  ✗ Error importing Gradio demo: {e}")
        return False

def main():
    """Run all tests"""
    print("Multimodal Video Chatbot - Setup Test")
    print("=" * 50)
    
    tests = [
        ("Python Dependencies", check_python_dependencies),
        ("C++ Executables", check_executables),
        ("Model Files", check_models),
        ("WebSocket Server", test_websocket_server),
        ("Gradio Demo", test_gradio_demo)
    ]
    
    results = []
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"  ✗ Error in {test_name}: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 50)
    print("SUMMARY")
    print("=" * 50)
    
    passed = 0
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
    
    print(f"\nTests passed: {passed}/{len(results)}")
    
    if passed == len(results):
        print("\n🎉 All tests passed! Your setup is ready.")
        print("\nTo run the application:")
        print("1. Start server: ./build/websocket_server")
        print("2. Run GUI: ./build/VideoChatbot")
        print("3. Or run web demo: cd gradio_demo && python app.py")
    else:
        print("\n⚠ Some tests failed. Please check the issues above.")
        print("Refer to README.md for setup instructions.")

if __name__ == "__main__":
    main()




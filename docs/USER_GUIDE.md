# User Guide

## Video Captioning

1. Launch the WebSocket server:
   ```
   ./build/server/WebSocketServer
   ```

2. (Optional) Launch the Qt GUI:
   ```
   ./build/gui/VideoChatbot
   ```

3. In the Qt GUI, click "Select Directory" and choose a folder containing video files (.mp4, .mov).

4. Wait for captions to generate. Progress bar indicates overall progress.  
   Captions appear in the timeline widget as they arrive.

5. Click on any caption entry to play the video from that timestamp.

## Chat & Search

1. Switch to the "Chat & Search" tab in the Qt GUI.

2. Type a query in any supported language (e.g., English, Spanish, Mandarin).

3. Press the "Send" button. The chatbot response streams in real-time.

4. Retrieved segments (video @ timestamp) appear below. Double-click to play that segment.

## Gradio Web Demo

1. Navigate to `gradio_demo/`:
   ```
   cd gradio_demo
   source ../venv/bin/activate  # if virtual environment is used
   pip install -r requirements.txt
   python app.py
   ```

2. Open the provided URL in your browser (e.g., `http://localhost:7860`).

3. Use the "Video Captioning" tab to upload a video and view captions.

4. Use the "Chat & Search" tab to type multilingual queries, see chatbot replies, and clickable video segments.

## Configuration

- Modify `config.json` (to be created) to set:
  ```json
  {
    "translation_model": "Helsinki-NLP/opus-mt-<src>-en"
  }
  ```
- Place custom translation model names or paths here. Restart server to apply changes.
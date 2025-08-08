import subprocess
import json
import requests
import os
import sys
from pathlib import Path
from dotenv import load_dotenv
import openai
import whisper
import tempfile

# Get project root (parent of gradio_demo)
PROJECT_ROOT = Path(__file__).resolve().parent.parent

# Load .env from project root
env_path = PROJECT_ROOT / '.env'
if not env_path.exists():
    print(f"[ERROR] .env file not found at {env_path}. Please create it with your HUGGINGFACE_API_TOKEN.")
    sys.exit(1)
load_dotenv(dotenv_path=env_path)

# Load OpenAI API key
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")
if OPENAI_API_KEY:
    openai.api_key = OPENAI_API_KEY

# Configuration
HUGGINGFACE_API_TOKEN = os.getenv("HUGGINGFACE_API_TOKEN")
CAPTIONER_PATH = str(PROJECT_ROOT / "captioner")  # Absolute path to the compiled captioner executable
SERVER_URL = "http://localhost:5000"  # WebSocket server URL

def generate_captions(video_path, language="english"):
    print(f"[DEBUG] generate_captions called with video_path={video_path}, language={language}")
    try:
        # 1. Extract audio from video
        with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as audio_file:
            audio_path = audio_file.name
        ffmpeg_cmd = [
            'ffmpeg', '-y', '-i', video_path, '-ar', '16000', '-ac', '1', '-f', 'wav', audio_path
        ]
        subprocess.run(ffmpeg_cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"[DEBUG] Audio extracted to {audio_path}")
        # 2. Transcribe audio with Whisper
        model = whisper.load_model('base')
        result = model.transcribe(audio_path)
        transcript = result['text']
        print(f"[DEBUG] Whisper transcript: {transcript}")
        # 3. Translate transcript if needed
        if language.lower() != 'english' and OPENAI_API_KEY:
            prompt = (
                f"Translate the following transcript to {language}. Return only the translated text.\n\nTranscript:\n{transcript}"
            )
            print(f"[DEBUG] Calling OpenAI for translation to {language}")
            response = openai.chat.completions.create(
                model="gpt-3.5-turbo",
                messages=[{"role": "user", "content": prompt}],
                max_tokens=2048
            )
            translated = response.choices[0].message.content.strip()
            print(f"[DEBUG] OpenAI translated transcript: {translated}")
        else:
            translated = transcript
        # 4. Return as a single caption (timestamp 0.0)
        captions = [(0.0, translated)]
        return captions
    except Exception as e:
        print(f"[ERROR] Error in real transcription/translation: {e}")
        return generate_mock_captions(video_path)

def generate_mock_captions(video_path):
    """Generate mock captions for demo purposes"""
    # Extract video name for context
    video_name = Path(video_path).stem
    
    mock_captions = [
        (0.0, f"A person is shown in {video_name}"),
        (2.0, f"The person in {video_name} is speaking"),
        (4.0, f"The scene in {video_name} continues"),
        (6.0, f"More activity is shown in {video_name}"),
        (8.0, f"The video {video_name} concludes")
    ]
    
    return mock_captions

def query_chatbot(text_input):
    """Query the chatbot with text input"""
    if OPENAI_API_KEY:
        prompt = f"You are a multilingual video chatbot. Answer the following query: {text_input}"
        response = openai.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[{"role": "user", "content": prompt}],
            max_tokens=256
        )
        try:
            answer = response.choices[0].message.content
            return answer, []
        except Exception as e:
            print(f"Error parsing OpenAI chatbot response: {e}")
            return generate_mock_response(text_input), []
    try:
        # Prepare request payload
        payload = {
            "type": "query", 
            "text": text_input
        }
        
        # Send request to server
        response = requests.post(
            f"{SERVER_URL}/query",
            headers={"Content-Type": "application/json"},
            json=payload,
            timeout=30
        )
        
        if response.status_code == 200:
            data = response.json()
            return data.get("response", "No response received"), data.get("matches", [])
        else:
            print(f"Server error: {response.status_code}")
            return generate_mock_response(text_input), []
            
    except requests.exceptions.RequestException as e:
        print(f"Network error: {e}")
        return generate_mock_response(text_input), []
    except Exception as e:
        print(f"Error querying chatbot: {e}")
        return generate_mock_response(text_input), []

def generate_mock_response(query):
    """Generate a mock response for demo purposes"""
    return f"Based on your query '{query}', I found relevant video segments. This is a mock response for demonstration purposes."

def check_dependencies():
    """Check if all required dependencies are available"""
    issues = []
    
    # Check if captioner executable exists
    if not os.path.exists(CAPTIONER_PATH):
        issues.append(f"Captioner executable not found at {CAPTIONER_PATH}")
    
    # Check if server is running
    try:
        response = requests.get(f"{SERVER_URL}/health", timeout=5)
        if response.status_code != 200:
            issues.append(f"Server health check failed: {response.status_code}")
    except:
        issues.append("Server is not running or not accessible")
    
    # Check Hugging Face token
    if HUGGINGFACE_API_TOKEN == "<YOUR_HF_TOKEN>":
        issues.append("Hugging Face API token not set")
    
    return issues

if __name__ == "__main__":
    # Check dependencies when run directly
    issues = check_dependencies()
    if issues:
        print("Dependency issues found:")
        for issue in issues:
            print(f"  - {issue}")
        print("\nSome features may not work properly.")
    else:
        print("All dependencies are available.")
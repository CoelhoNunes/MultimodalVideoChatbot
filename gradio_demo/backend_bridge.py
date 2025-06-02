import subprocess
import json
import requests

HUGGINGFACE_API_TOKEN = "<YOUR_HF_TOKEN>"

def generate_captions(video_path):
    # Call C++ captioner
    proc = subprocess.run(["./captioner", "--video", video_path, "--output", "captions.json"], capture_output=True)
    with open("captions.json", "r") as f:
        data = json.load(f)
    # Return list of (timestamp, caption)
    return [(item["timestamp"], item["text"]) for item in data]

def query_chatbot(text_input):
    url = "http://localhost:5000/query"
    headers = {"Content-Type": "application/json"}
    payload = {"type": "query", "text": text_input}
    resp = requests.post(url, headers=headers, json=payload)
    data = resp.json()
    # data = {"response": <str>, "matches": [ {video, timestamp, caption}, ... ]}
    return data["response"], data["matches"]
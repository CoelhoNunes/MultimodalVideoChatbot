# API Reference

## WebSocket Message Schemas

### Client → Server

1. **Generate Captions**
```json
{
  "type": "generate_captions",
  "video_paths": ["/path/to/video1.mp4", "/path/to/video2.mp4"]
}
```

2. **Chat Query**
```json
{
  "type": "query",
  "text": "Show me the dog playing"
}
```

3. **Shutdown**
```json
{ "type": "shutdown" }
```

### Server → Client

1. **Progress Update**
```json
{
  "type": "progress",
  "video": "video1.mp4",
  "percent": 60
}
```

2. **Caption**
```json
{
  "type": "caption",
  "video": "video1.mp4",
  "timestamp": "00:01:10",
  "text": "A dog runs across the field"
}
```

3. **Response Chunk**
```json
{
  "type": "response_chunk",
  "text": "Sure, I see a dog..."
}
```

4. **Matches**
```json
{
  "type": "matches",
  "results": [
    { "video": "video1.mp4", "timestamp": "00:01:10", "caption": "A dog runs across the field" },
    { "video": "video2.mp4", "timestamp": "00:02:05", "caption": "Children playing soccer" }
  ]
}
```
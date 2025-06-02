import gradio as gr
from backend_bridge import generate_captions, query_chatbot

def caption_interface(video_file):
    captions = generate_captions(video_file.name)
    timeline_html = "<ul>"
    for t, c in captions:
        timeline_html += f"<li><b>{t}</b>: {c}</li>"
    timeline_html += "</ul>"
    return timeline_html

def chat_interface(user_text):
    response, matches = query_chatbot(user_text)
    match_html = "<ul>"
    for m in matches:
        match_html += f'<li><a target="_blank" href="/?video={m["video"]}&time={m["timestamp"]}">{m["video"]} @ {m["timestamp"]}</a>: {m["caption"]}</li>'
    match_html += "</ul>"
    return response, match_html

with gr.Blocks() as demo:
    gr.Markdown("# Video Captioning & Multilingual Chatbot Demo")
    with gr.Tab("Video Captioning"):
        vid_in = gr.Video(label="Upload Video")
        cap_out = gr.HTML(label="Generated Captions Timeline")
        vid_btn = gr.Button("Generate Captions")
        vid_btn.click(fn=caption_interface, inputs=vid_in, outputs=cap_out)

    with gr.Tab("Chat & Search"):
        txt_in = gr.Textbox(label="Enter your query (any language)")
        resp_out = gr.Textbox(label="Chatbot Response")
        match_out = gr.HTML(label="Matched Video Segments")
        chat_btn = gr.Button("Ask")
        chat_btn.click(fn=chat_interface, inputs=txt_in, outputs=[resp_out, match_out])

demo.launch(server_name="0.0.0.0", server_port=7860)
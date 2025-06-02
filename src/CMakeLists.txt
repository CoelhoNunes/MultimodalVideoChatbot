add_library(captioning STATIC
    captioning/Captioner.cpp
    captioning/Captioner.h
)

target_link_libraries(captioning
    PRIVATE FFmpeg::avformat FFmpeg::avcodec FFmpeg::avutil FFmpeg::swscale
            ONNXRuntime::onnxruntime
)

add_library(embedding STATIC
    embedding/Embedder.cpp
    embedding/Embedder.h
)

target_link_libraries(embedding
    PRIVATE ONNXRuntime::onnxruntime faiss
)

add_library(chatbot STATIC
    chatbot/Chatbot.cpp
    chatbot/Chatbot.h
)

target_link_libraries(chatbot
    PRIVATE fastText::fastText ONNXRuntime::onnxruntime
)

add_library(server STATIC
    server/WebSocketServer.cpp
    server/WebSocketServer.h
)

target_link_libraries(server
    PRIVATE Boost::asio Qt5::Core Qt5::Network Qt5::WebSockets
)

if (BUILD_GUI)
    add_executable(VideoChatbot
        gui/main.cpp
        gui/CaptionTab.cpp
        gui/CaptionTab.h
        gui/ChatTab.cpp
        gui/ChatTab.h
    )

    target_link_libraries(VideoChatbot
        PRIVATE captioning embedding chatbot server Qt5::Widgets Qt5::Multimedia
                Qt5::Network Qt5::WebSockets
    )
endif()

add_executable(captioner
    captioning/Captioner.cpp
    captioning/Captioner.h
    embedding/Embedder.cpp
    embedding/Embedder.h
)

target_link_libraries(captioner
    PRIVATE captioning embedding faiss ONNXRuntime::onnxruntime
)
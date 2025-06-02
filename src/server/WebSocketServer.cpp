#include "WebSocketServer.h"
// TODO: Implement Boost.Beast or Boost.Asio WebSocket server

WebSocketServer::WebSocketServer(unsigned short port) : port_(port) {}

void WebSocketServer::run() {
    serverThread_ = std::thread(&WebSocketServer::doListen, this);
}

void WebSocketServer::stop() {
    // TODO: Signal server to stop, join thread
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void WebSocketServer::doListen() {
    // TODO: Listen on port_, accept WebSocket connections, parse JSON messages
    // Handle "generate_captions", "query", "shutdown"
}
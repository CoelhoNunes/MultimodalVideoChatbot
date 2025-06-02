#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <thread>
#include <string>

class WebSocketServer {
public:
    WebSocketServer(unsigned short port);
    void run();
    void stop();
private:
    unsigned short port_;
    std::thread serverThread_;
    void doListen();
    // TODO: Add handlers for WebSocket connections and messages
};
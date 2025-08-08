#include "WebSocketServer.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<WebSocketServer> server;

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << std::endl;
    if (server) {
        server->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Create WebSocket server
    server = std::make_unique<WebSocketServer>(8080);
    
    // Start server
    if (!server->start()) {
        std::cerr << "Failed to start WebSocket server" << std::endl;
        return 1;
    }
    
    std::cout << "WebSocket server started on port 8080" << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    // Keep server running
    while (server->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}



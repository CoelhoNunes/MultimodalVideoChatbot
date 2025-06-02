#include <QApplication>
#include <QTabWidget>
#include "CaptionTab.h"
#include "ChatTab.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QTabWidget tabs;
    tabs.addTab(new CaptionTab(), "Video Captioning");
    tabs.addTab(new ChatTab(), "Chat & Search");
    tabs.setWindowTitle("Multimodal Video Captioning & Chatbot");
    tabs.resize(800, 600);
    tabs.show();
    return app.exec();
}
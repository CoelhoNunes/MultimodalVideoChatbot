#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <iostream>
#include "CaptionTab.h"
#include "ChatTab.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Multimodal Video Chatbot");
        setMinimumSize(800, 600);
        
        setupUI();
        setupMenuBar();
        setupStatusBar();
        
        // Connect signals between tabs
        connect(chatTab_, &ChatTab::seekVideo, this, &MainWindow::onSeekVideo);
    }

private slots:
    void onSeekVideo(const QString& video, double timestamp) {
        // Switch to caption tab and seek to the specified timestamp
        tabWidget_->setCurrentIndex(0); // Switch to caption tab
        
        // TODO: Implement seeking in caption tab
        // This would require adding a method to CaptionTab to seek to a specific timestamp
        std::cout << "Seeking to video: " << video.toStdString() 
                  << " at timestamp: " << timestamp << std::endl;
    }

private:
    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        
        // Create tab widget
        tabWidget_ = new QTabWidget(this);
        
        // Create tabs
        captionTab_ = new CaptionTab(this);
        chatTab_ = new ChatTab(this);
        
        // Add tabs to widget
        tabWidget_->addTab(captionTab_, "Video Captioning");
        tabWidget_->addTab(chatTab_, "Chat & Search");
        
        layout->addWidget(tabWidget_);
    }
    
    void setupMenuBar() {
        QMenuBar* menuBar = this->menuBar();
        
        // File menu
        QMenu* fileMenu = menuBar->addMenu("&File");
        QAction* exitAction = fileMenu->addAction("E&xit");
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        
        // Help menu
        QMenu* helpMenu = menuBar->addMenu("&Help");
        QAction* aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    }
    
    void setupStatusBar() {
        statusBar()->showMessage("Ready");
    }
    
    void showAbout() {
        QMessageBox::about(this, "About", 
            "Multimodal Video Chatbot\n\n"
            "A desktop application for video captioning and multilingual chat.\n"
            "Supports multiple languages and real-time video analysis.");
    }
    
    QTabWidget* tabWidget_;
    CaptionTab* captionTab_;
    ChatTab* chatTab_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Multimodal Video Chatbot");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("VideoChatbot");
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    std::cout << "Multimodal Video Chatbot started" << std::endl;
    std::cout << "Make sure the WebSocket server is running on port 8080" << std::endl;
    
    return app.exec();
}

#include "main.moc"
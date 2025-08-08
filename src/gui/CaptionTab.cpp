#include "CaptionTab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebSocket>
#include <QMessageBox>
#include <iostream>

CaptionTab::CaptionTab(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupWebSocket();
}

CaptionTab::~CaptionTab() {
    if (webSocket_) {
        webSocket_->close();
    }
}

void CaptionTab::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Video selection section
    QHBoxLayout* videoLayout = new QHBoxLayout();
    videoPathLabel_ = new QLabel("No video selected");
    QPushButton* selectVideoBtn = new QPushButton("Select Video");
    videoLayout->addWidget(videoPathLabel_);
    videoLayout->addWidget(selectVideoBtn);
    mainLayout->addLayout(videoLayout);
    
    // Video player
    mediaPlayer_ = new QMediaPlayer(this);
    videoWidget_ = new QVideoWidget(this);
    mediaPlayer_->setVideoOutput(videoWidget_);
    mainLayout->addWidget(videoWidget_);
    
    // Controls
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    generateCaptionsBtn_ = new QPushButton("Generate Captions");
    progressBar_ = new QProgressBar();
    progressBar_->setVisible(false);
    controlsLayout->addWidget(generateCaptionsBtn_);
    controlsLayout->addWidget(progressBar_);
    mainLayout->addLayout(controlsLayout);
    
    // Timeline
    timelineWidget_ = new QListWidget(this);
    timelineWidget_->setMaximumHeight(200);
    mainLayout->addWidget(timelineWidget_);
    
    // Connect signals
    connect(selectVideoBtn, &QPushButton::clicked, this, &CaptionTab::selectVideo);
    connect(generateCaptionsBtn_, &QPushButton::clicked, this, &CaptionTab::generateCaptions);
    connect(timelineWidget_, &QListWidget::itemDoubleClicked, this, &CaptionTab::onTimelineItemClicked);
    connect(mediaPlayer_, &QMediaPlayer::positionChanged, this, &CaptionTab::onVideoPositionChanged);
}

void CaptionTab::setupWebSocket() {
    webSocket_ = new QWebSocket();
    
    connect(webSocket_, &QWebSocket::connected, this, &CaptionTab::onWebSocketConnected);
    connect(webSocket_, &QWebSocket::disconnected, this, &CaptionTab::onWebSocketDisconnected);
    connect(webSocket_, &QWebSocket::textMessageReceived, this, &CaptionTab::onWebSocketMessageReceived);
    
    // Connect to WebSocket server
    webSocket_->open(QUrl("ws://localhost:8080"));
}

void CaptionTab::selectVideo() {
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Select Video File", "", "Video Files (*.mp4 *.avi *.mov *.mkv)");
    
    if (!fileName.isEmpty()) {
        videoPath_ = fileName;
        videoPathLabel_->setText(QFileInfo(fileName).fileName());
        
        // Load video into media player
        mediaPlayer_->setMedia(QUrl::fromLocalFile(fileName));
        
        // Enable caption generation
        generateCaptionsBtn_->setEnabled(true);
    }
}

void CaptionTab::generateCaptions() {
    if (videoPath_.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a video file first.");
        return;
    }
    
    // Show progress
    progressBar_->setVisible(true);
    progressBar_->setRange(0, 0); // Indeterminate progress
    generateCaptionsBtn_->setEnabled(false);
    
    // Clear previous captions
    timelineWidget_->clear();
    
    // Send caption request via WebSocket
    QJsonObject request;
    request["type"] = "caption_request";
    request["video_path"] = videoPath_;
    
    QJsonDocument doc(request);
    webSocket_->sendTextMessage(doc.toJson());
}

void CaptionTab::onWebSocketConnected() {
    std::cout << "Connected to WebSocket server" << std::endl;
}

void CaptionTab::onWebSocketDisconnected() {
    std::cout << "Disconnected from WebSocket server" << std::endl;
}

void CaptionTab::onWebSocketMessageReceived(const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull()) {
        std::cerr << "Invalid JSON message received" << std::endl;
        return;
    }
    
    QJsonObject jsonObj = doc.object();
    QString type = jsonObj["type"].toString();
    
    if (type == "caption_response") {
        handleCaptionResponse(jsonObj);
    }
}

void CaptionTab::handleCaptionResponse(const QJsonObject& response) {
    QString status = response["status"].toString();
    
    if (status == "processing") {
        // Update progress
        progressBar_->setValue(50);
    } else if (status == "completed") {
        // Hide progress
        progressBar_->setVisible(false);
        generateCaptionsBtn_->setEnabled(true);
        
        // Parse captions
        QJsonArray captions = response["captions"].toArray();
        for (const QJsonValue& captionValue : captions) {
            QJsonObject caption = captionValue.toObject();
            
            double timestamp = caption["timestamp"].toDouble();
            QString text = caption["text"].toString();
            double confidence = caption["confidence"].toDouble();
            
            // Add to timeline
            QString itemText = QString("[%1s] %2 (%.1f%%)")
                .arg(timestamp, 0, 'f', 1)
                .arg(text)
                .arg(confidence * 100);
            
            QListWidgetItem* item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, timestamp);
            timelineWidget_->addItem(item);
        }
        
        // Store captions for later use
        captions_.clear();
        for (const QJsonValue& captionValue : captions) {
            QJsonObject caption = captionValue.toObject();
            CaptionResult result;
            result.timestamp = caption["timestamp"].toDouble();
            result.text = caption["text"].toString().toStdString();
            result.confidence = caption["confidence"].toDouble();
            captions_.push_back(result);
        }
    }
}

void CaptionTab::onTimelineItemClicked(QListWidgetItem* item) {
    // Get timestamp from item data
    double timestamp = item->data(Qt::UserRole).toDouble();
    
    // Seek video to timestamp
    qint64 position = static_cast<qint64>(timestamp * 1000); // Convert to milliseconds
    mediaPlayer_->setPosition(position);
    
    // Start playing
    mediaPlayer_->play();
}

void CaptionTab::onVideoPositionChanged(qint64 position) {
    // Update timeline selection based on current video position
    double currentTime = position / 1000.0; // Convert from milliseconds
    
    for (int i = 0; i < timelineWidget_->count(); ++i) {
        QListWidgetItem* item = timelineWidget_->item(i);
        double itemTimestamp = item->data(Qt::UserRole).toDouble();
        
        // Highlight current caption (within 1 second tolerance)
        if (std::abs(currentTime - itemTimestamp) < 1.0) {
            timelineWidget_->setCurrentItem(item);
            break;
        }
    }
}

const std::vector<CaptionResult>& CaptionTab::getCaptions() const {
    return captions_;
}
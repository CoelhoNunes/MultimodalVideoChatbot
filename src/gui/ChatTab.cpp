#include "ChatTab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebSocket>
#include <QMessageBox>
#include <iostream>

ChatTab::ChatTab(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupWebSocket();
}

ChatTab::~ChatTab() {
    if (webSocket_) {
        webSocket_->close();
    }
}

void ChatTab::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Chat input section
    QHBoxLayout* inputLayout = new QHBoxLayout();
    queryInput_ = new QTextEdit(this);
    queryInput_->setMaximumHeight(100);
    queryInput_->setPlaceholderText("Enter your query in any language...");
    
    sendButton_ = new QPushButton("Send", this);
    inputLayout->addWidget(queryInput_);
    inputLayout->addWidget(sendButton_);
    mainLayout->addLayout(inputLayout);
    
    // Response section
    responseBrowser_ = new QTextEdit(this);
    responseBrowser_->setReadOnly(true);
    responseBrowser_->setMaximumHeight(200);
    mainLayout->addWidget(responseBrowser_);
    
    // Matches section
    QLabel* matchesLabel = new QLabel("Matched Video Segments:", this);
    mainLayout->addWidget(matchesLabel);
    
    matchesList_ = new QListWidget(this);
    matchesList_->setMaximumHeight(200);
    mainLayout->addWidget(matchesList_);
    
    // Connect signals
    connect(sendButton_, &QPushButton::clicked, this, &ChatTab::sendQuery);
    connect(queryInput_, &QTextEdit::textChanged, this, &ChatTab::onTextChanged);
    connect(matchesList_, &QListWidget::itemDoubleClicked, this, &ChatTab::onMatchItemClicked);
}

void ChatTab::setupWebSocket() {
    webSocket_ = new QWebSocket();
    
    connect(webSocket_, &QWebSocket::connected, this, &ChatTab::onWebSocketConnected);
    connect(webSocket_, &QWebSocket::disconnected, this, &ChatTab::onWebSocketDisconnected);
    connect(webSocket_, &QWebSocket::textMessageReceived, this, &ChatTab::onWebSocketMessageReceived);
    
    // Connect to WebSocket server
    webSocket_->open(QUrl("ws://localhost:8080"));
}

void ChatTab::sendQuery() {
    QString query = queryInput_->toPlainText().trimmed();
    
    if (query.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a query.");
        return;
    }
    
    // Disable send button during processing
    sendButton_->setEnabled(false);
    
    // Add user query to response browser
    responseBrowser_->append(QString("<b>You:</b> %1").arg(query));
    
    // Send query via WebSocket
    QJsonObject request;
    request["type"] = "chat_query";
    request["query"] = query;
    
    QJsonDocument doc(request);
    webSocket_->sendTextMessage(doc.toJson());
    
    // Clear input
    queryInput_->clear();
}

void ChatTab::onTextChanged() {
    // Enable/disable send button based on input
    QString text = queryInput_->toPlainText().trimmed();
    sendButton_->setEnabled(!text.isEmpty());
}

void ChatTab::onWebSocketConnected() {
    std::cout << "ChatTab connected to WebSocket server" << std::endl;
}

void ChatTab::onWebSocketDisconnected() {
    std::cout << "ChatTab disconnected from WebSocket server" << std::endl;
}

void ChatTab::onWebSocketMessageReceived(const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull()) {
        std::cerr << "Invalid JSON message received" << std::endl;
        return;
    }
    
    QJsonObject jsonObj = doc.object();
    QString type = jsonObj["type"].toString();
    
    if (type == "chat_response") {
        handleChatResponse(jsonObj);
    }
}

void ChatTab::handleChatResponse(const QJsonObject& response) {
    QString status = response["status"].toString();
    
    if (status == "processing") {
        // Show processing message
        responseBrowser_->append("<i>Processing your query...</i>");
    } else if (status == "completed") {
        // Remove processing message and show response
        QString currentText = responseBrowser_->toHtml();
        currentText.replace("<i>Processing your query...</i>", "");
        responseBrowser_->setHtml(currentText);
        
        // Add bot response
        QString botResponse = response["response"].toString();
        QString detectedLang = response["detected_language"].toString();
        
        responseBrowser_->append(QString("<b>Bot (%1):</b> %2")
            .arg(detectedLang)
            .arg(botResponse));
        
        // Parse and display matches
        QJsonArray matches = response["matches"].toArray();
        matchesList_->clear();
        
        for (const QJsonValue& matchValue : matches) {
            QJsonObject match = matchValue.toObject();
            
            QString video = match["video"].toString();
            double timestamp = match["timestamp"].toDouble();
            QString caption = match["caption"].toString();
            double similarity = match["similarity"].toDouble();
            
            QString itemText = QString("[%1s] %2 (%.1f%%)")
                .arg(timestamp, 0, 'f', 1)
                .arg(caption)
                .arg(similarity * 100);
            
            QListWidgetItem* item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, QVariant::fromValue(QPair<QString, double>(video, timestamp)));
            matchesList_->addItem(item);
        }
        
        // Re-enable send button
        sendButton_->setEnabled(true);
    }
}

void ChatTab::onMatchItemClicked(QListWidgetItem* item) {
    // Get video and timestamp from item data
    QPair<QString, double> data = item->data(Qt::UserRole).value<QPair<QString, double>>();
    QString video = data.first;
    double timestamp = data.second;
    
    // Emit signal to seek video in CaptionTab
    emit seekVideo(video, timestamp);
}

void ChatTab::setCaptions(const std::vector<CaptionResult>& captions) {
    captions_ = captions;
}

const std::vector<CaptionResult>& ChatTab::getCaptions() const {
    return captions_;
}
#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QWebSocket>
#include <QString>
#include <vector>

// Forward declaration
struct CaptionResult;

class ChatTab : public QWidget {
    Q_OBJECT

public:
    explicit ChatTab(QWidget* parent = nullptr);
    ~ChatTab();
    
    void setCaptions(const std::vector<CaptionResult>& captions);
    const std::vector<CaptionResult>& getCaptions() const;

signals:
    void seekVideo(const QString& video, double timestamp);

private slots:
    void sendQuery();
    void onTextChanged();
    void onMatchItemClicked(QListWidgetItem* item);
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketMessageReceived(const QString& message);

private:
    void setupUI();
    void setupWebSocket();
    void handleChatResponse(const QJsonObject& response);
    
    QTextEdit* queryInput_;
    QPushButton* sendButton_;
    QTextEdit* responseBrowser_;
    QListWidget* matchesList_;
    QWebSocket* webSocket_;
    
    std::vector<CaptionResult> captions_;
};
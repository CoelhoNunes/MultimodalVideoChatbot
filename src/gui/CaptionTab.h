#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QWebSocket>
#include <QString>
#include <vector>

// Forward declaration
struct CaptionResult;

class CaptionTab : public QWidget {
    Q_OBJECT

public:
    explicit CaptionTab(QWidget* parent = nullptr);
    ~CaptionTab();
    
    const std::vector<CaptionResult>& getCaptions() const;

private slots:
    void selectVideo();
    void generateCaptions();
    void onTimelineItemClicked(QListWidgetItem* item);
    void onVideoPositionChanged(qint64 position);
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketMessageReceived(const QString& message);

private:
    void setupUI();
    void setupWebSocket();
    void handleCaptionResponse(const QJsonObject& response);
    
    QLabel* videoPathLabel_;
    QPushButton* generateCaptionsBtn_;
    QProgressBar* progressBar_;
    QListWidget* timelineWidget_;
    QMediaPlayer* mediaPlayer_;
    QVideoWidget* videoWidget_;
    QWebSocket* webSocket_;
    
    QString videoPath_;
    std::vector<CaptionResult> captions_;
};
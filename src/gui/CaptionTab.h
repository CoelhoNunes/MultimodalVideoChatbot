#pragma once

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTreeWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QWebSocket>

class CaptionTab : public QWidget {
    Q_OBJECT
public:
    CaptionTab(QWidget *parent = nullptr);
private slots:
    void onSelectDirectory();
    void onWebSocketMessage(const QString &message);
    void onTimelineItemClicked(QTreeWidgetItem *item, int column);
private:
    QPushButton *selectDirButton;
    QProgressBar *progressBar;
    QTreeWidget *timelineWidget;
    QVideoWidget *videoWidget;
    QMediaPlayer *mediaPlayer;
    QWebSocket *socket;
    // TODO: Map timeline items to (video, timestamp) pairs
};
#include "CaptionTab.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QTreeWidgetItem>

CaptionTab::CaptionTab(QWidget *parent) : QWidget(parent) {
    selectDirButton = new QPushButton("Select Directory");
    progressBar = new QProgressBar;
    timelineWidget = new QTreeWidget;
    timelineWidget->setColumnCount(2);
    timelineWidget->setHeaderLabels({"Timestamp", "Caption"});
    videoWidget = new QVideoWidget;
    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setVideoOutput(videoWidget);

    socket = new QWebSocket;
    socket->open(QUrl("ws://127.0.0.1:5000"));
    connect(socket, &QWebSocket::textMessageReceived, this, &CaptionTab::onWebSocketMessage);

    connect(selectDirButton, &QPushButton::clicked, this, &CaptionTab::onSelectDirectory);
    connect(timelineWidget, &QTreeWidget::itemClicked, this, &CaptionTab::onTimelineItemClicked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(selectDirButton);
    layout->addWidget(progressBar);
    layout->addWidget(timelineWidget);
    layout->addWidget(videoWidget);
    setLayout(layout);
}

void CaptionTab::onSelectDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Video Directory");
    if (dir.isEmpty()) return;
    // TODO: Collect video file paths and send JSON over WebSocket
}

void CaptionTab::onWebSocketMessage(const QString &message) {
    // TODO: Parse JSON, update progressBar or add caption to timelineWidget
}

void CaptionTab::onTimelineItemClicked(QTreeWidgetItem *item, int column) {
    // TODO: Seek mediaPlayer to timestamp associated with this item and play
}
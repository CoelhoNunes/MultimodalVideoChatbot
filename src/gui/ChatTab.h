#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QListWidget>
#include <QWebSocket>

class ChatTab : public QWidget {
    Q_OBJECT
public:
    ChatTab(QWidget *parent = nullptr);
private slots:
    void onSendMessage();
    void onWebSocketMessage(const QString &message);
    void onMatchItemDoubleClicked(QListWidgetItem *item);
private:
    QTextEdit *inputEdit;
    QPushButton *sendButton;
    QTextBrowser *responseBrowser;
    QListWidget *matchesList;
    QWebSocket *socket;
    // TODO: Map list items to (video, timestamp) pairs
};
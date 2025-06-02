#include "ChatTab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>

ChatTab::ChatTab(QWidget *parent) : QWidget(parent) {
    inputEdit = new QTextEdit;
    sendButton = new QPushButton("Send");
    responseBrowser = new QTextBrowser;
    matchesList = new QListWidget;

    socket = new QWebSocket;
    socket->open(QUrl("ws://127.0.0.1:5000"));
    connect(socket, &QWebSocket::textMessageReceived, this, &ChatTab::onWebSocketMessage);

    connect(sendButton, &QPushButton::clicked, this, &ChatTab::onSendMessage);
    connect(matchesList, &QListWidget::itemDoubleClicked, this, &ChatTab::onMatchItemDoubleClicked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputEdit);
    layout->addWidget(sendButton);
    layout->addWidget(responseBrowser);
    layout->addWidget(matchesList);
    setLayout(layout);
}

void ChatTab::onSendMessage() {
    QString text = inputEdit->toPlainText();
    if (text.isEmpty()) return;
    QJsonObject msg;
    msg["type"] = "query";
    msg["text"] = text;
    socket->sendTextMessage(QJsonDocument(msg).toJson());
    inputEdit->clear();
    responseBrowser->clear();
    matchesList->clear();
}

void ChatTab::onWebSocketMessage(const QString &message) {
    // TODO: Parse JSON, append response chunks to responseBrowser, populate matchesList
}

void ChatTab::onMatchItemDoubleClicked(QListWidgetItem *item) {
    // TODO: Seek video in CaptionTab or open popup to play video at the timestamp
}
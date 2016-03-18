#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QListView>
#include <QStackedWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QTcpSocket>
#include <QTime>

#include "inputdialog.h"

class ChatClient;

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QString _userName, QWidget *_parent);
    ~ChatWidget();

    QTextEdit *txtInfo;
    QTextEdit *txtInput;

private slots:
    void slotSendToServer();

private:
    ChatClient *chatClient;
};

class ChatClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatClient(QWidget *_parent = 0);
    ~ChatClient();

    void sendToServer(const QString &_addressee, const QString &_message);

private slots:
    void slotConnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError _err);

    void showListClients();
    void slotSetCurrentChat(QModelIndex _modelIndex);

private:
    int indexOfChat(QString _str);

    QTcpSocket *tcpSocket;
    quint16 nextBlockSize;

    QLabel *addressee;
    QStringListModel model;
};

#endif // CHATCLIENT_H

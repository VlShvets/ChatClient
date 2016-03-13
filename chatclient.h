#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QApplication>
#include <QMainWindow>
#include <QTcpSocket>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTime>
#include <QInputDialog>
#include <QDockWidget>
#include <QTableView>
#include <QStringListModel>

#include "inputdialog.h"

class ChatClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatClient(QWidget *_parent = 0);
    ~ChatClient();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError _err);
    void slotSendToServer();
    void slotConnected();

private:
    QTcpSocket *tcpSocket;
    QTextEdit *txtInfo;
    QTextEdit *txtInput;
    quint16 nextBlockSize;
    QStringList *slClients;
};

#endif // CHATCLIENT_H

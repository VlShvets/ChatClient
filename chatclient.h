#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QApplication>
#include <QMainWindow>
#include <QTcpSocket>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QListView>
#include <QDockWidget>
#include <QTime>
#include <QItemSelectionModel>

#include "inputdialog.h"
#include "chatwidget.h"

class ChatClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatClient(QWidget *_parent = 0);
    ~ChatClient();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError _err);
    void slotSendToServer(const QString &_addressee, const QString &_message);
    void slotConnected();

private:
    QTcpSocket *tcpSocket;
    quint16 nextBlockSize;

    QStringListModel model;
    QStringList slClients;
    QTabWidget *tabWidget;
};

#endif // CHATCLIENT_H

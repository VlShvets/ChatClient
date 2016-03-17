#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QApplication>
#include <QMainWindow>
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

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QString _userName, QWidget *_parent);
    ~ChatWidget();

    QTextEdit *txtInfo;
    QTextEdit *txtInput;
};

class ChatClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatClient(QWidget *_parent = 0);
    ~ChatClient();

private slots:
    void slotConnected();
    void slotReadyRead();
    void slotSendToServer();
    void slotError(QAbstractSocket::SocketError _err);

    void slotSetCurrentChat(QModelIndex _modelIndex);

private:
    void sendToServer(const QString &_addressee, const QString &_message);
    int indexOfChat(QString _str);

    QTcpSocket *tcpSocket;
    quint16 nextBlockSize;

    QStringListModel model;
    QStackedWidget *stackedWidget;
};

#endif // CHATCLIENT_H

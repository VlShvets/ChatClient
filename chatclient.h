#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QApplication>
#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>

class ChatClient : public QWidget
{
    Q_OBJECT

public:
    ChatClient(const QString &_strHost, int _nPort, QWidget *_parent = 0);
    ~ChatClient();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError _err);
    void slotSendToServer();
    void slotConnected();

private:
    QTcpSocket *tcpSocket;
    QTextEdit *txtInfo;
    QLineEdit *txtInput;
    quint16 nextBlockSize;
};

#endif // CHATCLIENT_H

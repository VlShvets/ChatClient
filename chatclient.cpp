#include "chatclient.h"

ChatClient::ChatClient(const QString &_strHost, int _nPort, QWidget *_parent)
    : QWidget(_parent), nextBlockSize(0)
{
    tcpSocket = new QTcpSocket(this);

    tcpSocket->connectToHost(_strHost, _nPort);
    connect(tcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    txtInfo = new QTextEdit;
    txtInput = new QLineEdit;

    txtInfo->setReadOnly(true);

    QPushButton *pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
    connect(txtInput, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));

    QVBoxLayout *vbxLayout = new QVBoxLayout;
    vbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    vbxLayout->addWidget(txtInfo);
    vbxLayout->addWidget(txtInput);
    vbxLayout->addWidget(pcmd);
    setLayout(vbxLayout);
}

ChatClient::~ChatClient()
{

}

void ChatClient::slotReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_5);
    for(;;)
    {
        if(!nextBlockSize)
        {
            if(tcpSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if(tcpSocket->bytesAvailable() < nextBlockSize)
            break;

        QTime time;
        QString str;
        in >> time >> str;

        txtInfo->append(time.toString() + " " + str);
        nextBlockSize = 0;
    }
}

void ChatClient::slotError(QAbstractSocket::SocketError _err)
{
    QString strError = "Error: " + (_err == QAbstractSocket::HostNotFoundError ? "The host was not found." :
                                    _err == QAbstractSocket::RemoteHostClosedError ? "The remote host is closed." :
                                    _err == QAbstractSocket::ConnectionRefusedError ? "The connection was refused." :
                                    QString(tcpSocket->errorString()));
    txtInfo->append(strError);
}

void ChatClient::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out << quint16(0) << QTime::currentTime() << txtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
    txtInput->setText("");
}

void ChatClient::slotConnected()
{
    txtInfo->append("Received the connected() signal");
}

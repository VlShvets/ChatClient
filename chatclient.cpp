#include "chatclient.h"

ChatClient::ChatClient(QWidget *_parent)
    : QMainWindow(_parent), nextBlockSize(0)
{
    tcpSocket = new QTcpSocket(this);

    QString hostName = "localhost";
    quint16 nPort = 2323;
    InputDialog *inputDialog = new InputDialog;
    if(inputDialog->exec() == QDialog::Accepted)
    {
        hostName = inputDialog->hostName();
        nPort = inputDialog->port();
    }
    delete inputDialog;

    bool bOk;
    QString clientName = QInputDialog::getText(0, tr("Имя пользователя"), tr("Введите имя пользователя:"),
                                        QLineEdit::Normal, "unnamed", &bOk);
    if(!bOk || clientName.isEmpty())
        clientName = "unnamed";
    this->setWindowTitle(clientName);
    tcpSocket->setObjectName(clientName);

    tcpSocket->connectToHost(hostName, nPort);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));


    QListView *lvClients = new QListView;
    lvClients->setModel(&model);
    lvClients->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setCentralWidget(lvClients);

    tabWidget = new QTabWidget;

    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setWindowTitle("Чат");
    dockWidget->setWidget(tabWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

ChatClient::~ChatClient()
{
    delete tabWidget;
    delete tcpSocket;
}

void ChatClient::slotReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);
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

        QString sender;
        QTime time;
        QString str;
        in >> sender >> time >> str;
        if(tcpSocket->objectName() == sender)
        {
            slClients = str.split("0x00", QString::SkipEmptyParts);
            slClients.removeAll(tcpSocket->objectName());
            model.setStringList(slClients);
        }

//        txtInfo->append(time.toString() + " " + str);
        nextBlockSize = 0;
    }
}

void ChatClient::slotError(QAbstractSocket::SocketError _err)
{
    QString strError = _err == QAbstractSocket::HostNotFoundError ? tr("Хост не был найден.") :
                       _err == QAbstractSocket::RemoteHostClosedError ? tr("Удаленный хост закрыт.") :
                       _err == QAbstractSocket::ConnectionRefusedError ? tr("В соединении было отказано.") :
                       QString(tcpSocket->errorString());
    QMessageBox::critical(0, tr("Ошибка"), tr("Невозможно установить соединение:") + " " + strError);
}

void ChatClient::slotSendToServer(const QString &_addressee, const QString &_message)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << quint16(0) << _addressee <<  QTime::currentTime() << _message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void ChatClient::slotConnected()
{
    slotSendToServer(tcpSocket->objectName(), " ");
}

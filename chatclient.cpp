#include "chatclient.h"

ChatClient::ChatClient(QWidget *_parent)
    : QMainWindow(_parent), nextBlockSize(0)
{
    this->setWindowTitle(tr("Чат"));

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
    tcpSocket->setObjectName(clientName);
    tcpSocket->connectToHost(hostName, nPort);

    connect(tcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    txtInfo = new QTextEdit;
    txtInput = new QTextEdit;
    slClients = new QStringList(clientName);

    txtInfo->setReadOnly(true);
//    tvClients->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    QPushButton *pcmd = new QPushButton(tr("&Отправить"));
    pcmd->setMaximumWidth(100);
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));

    QStringListModel *model = new QStringListModel;
    model->setStringList(*slClients);

    QTableView *tvClients = new QTableView;
    tvClients->setModel(model);
    tvClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tvClients->setHorizontalHeader(new QHorizontalHeader());

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *vbxLayout = new QVBoxLayout;
    vbxLayout->addWidget(new QLabel("<H1>Чат</H1>"), 0, Qt::AlignCenter);
    vbxLayout->addWidget(txtInfo, 10);
    vbxLayout->addWidget(txtInput, 1);
    vbxLayout->addWidget(pcmd, 0, Qt::AlignCenter);
    centralWidget->setLayout(vbxLayout);
    this->setCentralWidget(centralWidget);

    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setWidget(tvClients);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    txtInfo->append(tr("Хост") + " " + hostName + ". " +
                    tr("Порт") + " " + QString::number(nPort) + ". " +
                    tr("Установка соединения с сервером..."));
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
    QString strError = tr("Ошибка:") + " " + (_err == QAbstractSocket::HostNotFoundError ? tr("Хост не был найден.") :
                                              _err == QAbstractSocket::RemoteHostClosedError ? tr("Удаленный хост закрыт.") :
                                              _err == QAbstractSocket::ConnectionRefusedError ? tr("В соединении было отказано.") :
                                              QString(tcpSocket->errorString()));
    txtInfo->append(strError);
}

void ChatClient::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    QString strMessage = txtInput->toPlainText();
    strMessage.replace("\n", "<br>");
    out << quint16(0) << QTime::currentTime() << strMessage;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
    txtInput->setText("");
}

void ChatClient::slotConnected()
{
    txtInput->setText(tcpSocket->objectName());
    slotSendToServer();
}

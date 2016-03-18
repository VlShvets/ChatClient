#include "chatclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ChatClient client;
    client.resize(200, 300);
    client.show();

    return a.exec();
}

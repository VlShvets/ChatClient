#include "chatclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ChatClient client;
    client.show();

    return a.exec();
}

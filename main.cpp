#include "chatclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ChatClient client("localhost", 2323);
    client.show();

    return a.exec();
}

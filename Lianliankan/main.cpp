#include "lianliankan.h"
#include <QtWidgets/QApplication>
#include <iostream>

#include "utils.h"
#include <interception.h>

using namespace std;


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Lianliankan w;
    w.show();
    return a.exec();
}

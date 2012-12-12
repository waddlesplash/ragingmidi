#include <QtGui/QApplication>
#include <QtGui/QPlastiqueStyle>
#include "Gui/MainWind.h"

int main(int argc, char *argv[])
{ /* // experimental GL backend for all widgets
#ifndef QT_NO_OPENGL
    QApplication::setGraphicsSystem("opengl");
#endif*/
    QApplication a(argc, argv);
    MainWind w;
    w.show();

    return a.exec();
}

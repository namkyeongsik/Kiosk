#include "kioskwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    KioskWindow kioskWindow;
    kioskWindow.show();
    return app.exec();
}

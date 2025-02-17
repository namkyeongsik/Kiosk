#include "kioskwindow.h"
#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // StyleSetting of all Buttons
    app.setStyleSheet("QPushButton { background-color: #036B3F; color: white; } QCheckBox::indicator:checked { background-color: #036B3F; content:'✔'; border-radius: 5px; }");

    // Execute Window
    KioskWindow kioskWindow;
    kioskWindow.show();
    return app.exec();
}

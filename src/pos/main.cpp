#include "poswindow.h"

#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    app.setPalette(palette);

    app.setStyleSheet(R"(
        QPushButton {
            background-color: #036B3F;
            color: white;
            font-size: 18px;
            border-radius: 8px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #028C50;
        }
        QLabel {
            font-size: 18px;
            color: black;
        }
        QDialog {
            background-color: white;
            border-radius: 10px;
            padding: 15px;
        }
    )");
    POSWindow posWindow;
    posWindow.show();
    return app.exec();
}

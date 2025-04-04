#include <QApplication>
#include <QFile>
#include <QLatin1String>

#include "main_window.h"
#include "login_window.h"

int main(int argc, char **argv) {
    QApplication app (argc, argv);

    QFile styles_file("../styles.qss");
    styles_file.open(QFile::ReadOnly);
    QString styles { QLatin1String(styles_file.readAll()) };
    app.setStyleSheet(styles);
    styles_file.close();

    MainWindow main_window;
    main_window.show();
    return app.exec();
}
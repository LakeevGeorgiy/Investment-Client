#pragma once

#include <QMainWindow>

class MainWindow : public QWidget {

    Q_OBJECT

public:

    MainWindow(QWidget* root = nullptr);

private:

    void ClickLogIntButton();

};

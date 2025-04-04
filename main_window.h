#pragma once

#include <QMainWindow>
#include <QLabel>
#include <memory>

#include "context.h"

class MainWindow : public QWidget {

    Q_OBJECT
private:

    QLabel* username;
    std::shared_ptr<Context> context_;

public slots:

    void WriteName();

public:

    MainWindow(QWidget* root = nullptr);

private:

    void ClickLogIntButton();
    void ClickRegistrationButton();

};

#pragma once

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>

#include "request.h"

class LoginWindow : public QDialog {

    Q_OBJECT

public:

    LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

signals:

    void sendRequest(QUrl url);

private slots:

    void startRequest();
    void onHttpRead(QByteArray data);
    void onHttpFinished();

private:

    QLineEdit* username_input;
    QLineEdit* password_input;
    QPushButton* login_button;
    QThread* network_thread;

    QScopedPointer<Request, QScopedPointerDeleter<Request>> request;
};
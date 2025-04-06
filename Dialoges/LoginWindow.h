#pragma once

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>
#include <QNetworkRequest>
#include <QLabel>
#include <memory>

#include "../Context.h"
#include "../Requests/Request.h"
#include "../MainWindow.h"

class LoginWindow : public QDialog {

    Q_OBJECT

public:

    LoginWindow(std::shared_ptr<Context> context, QWidget* parent = nullptr);
    ~LoginWindow();

signals:

    void sendRequest(QNetworkRequest request, QByteArray body);
    void UserLogin();

private slots:

    void startRequest();
    void onHttpRead(int status_code, QByteArray data);
    void onHttpFinished();

private:

    QLineEdit* username_input;
    QLineEdit* password_input;
    QLabel* authorization_problem;
    QPushButton* login_button;
    QThread* network_thread;

    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> request;
};
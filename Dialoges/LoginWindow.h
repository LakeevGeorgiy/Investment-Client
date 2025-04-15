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

class LoginWindow : public QWidget {

    Q_OBJECT

public:

    LoginWindow(std::shared_ptr<Context> context, QWidget* parent = nullptr);
    ~LoginWindow();

signals:

    void LoginRequest(QNetworkRequest request, QByteArray body);
    void UserLogin();

private slots:

    void StartRequest();
    void OnHttpRead(int status_code, QByteArray data);
    void onHttpFinished();

private:

    QLineEdit* username_input_;
    QLineEdit* password_input_;
    QLabel* authorization_problem_;
    QPushButton* login_button_;
    QThread* network_thread_;

    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> request;

    void LoginButtonClicked();
};
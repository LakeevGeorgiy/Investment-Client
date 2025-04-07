#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QThread>

#include "../Context.h"
#include "../Requests/Request.h"


class RegisterWindow : public QDialog {
    Q_OBJECT

public:

    RegisterWindow(std::shared_ptr<Context> context, QWidget* parent = nullptr);
    ~RegisterWindow();

signals:

    void RegisterRequest(QNetworkRequest request, QByteArray body);
    void UserLogin();

private slots:

    void StartRequest();
    void onHttpRead(int status_code, QByteArray data);
    void onHttpFinished();

private:

    QLineEdit* username_input_;
    QLineEdit* password_input_;
    QLineEdit* repeat_pass_input_;
    QLabel* authorization_problem_;
    QPushButton* register_button_;
    QThread* network_thread_;

    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> request_;

    void RegisterButtonClicked();
};
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>
#include <QGridLayout>

#include "LoginWindow.h"

LoginWindow::LoginWindow( std::shared_ptr<Context> context, QWidget *parent): 
    QDialog(parent) 
    , username_input(new QLineEdit(this))
    , password_input(new QLineEdit(this))
    , authorization_problem(new QLabel(""))
    , login_button(new QPushButton("Log In", this))
    , network_thread(new QThread(this))
    , context_(context)
    , request(new Request())
{

    setWindowTitle(tr("Log In"));
    resize(400,200);
    
    auto top_layout = new QVBoxLayout(this);
    auto grid_layout = new QGridLayout();
    auto button_layout = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    auto username_label = new QLabel("Login: ");
    username_label->setObjectName("username_label");
    grid_layout->addWidget(username_label, 0, 0);

    username_input->setObjectName("username_input");
    grid_layout->addWidget(username_input, 0, 1);

    auto password_label = new QLabel("Password: ");
    password_label->setObjectName("password_label");
    grid_layout->addWidget(password_label, 1, 0);

    password_input->setObjectName("password_input");
    password_input->setEchoMode(QLineEdit::Password);
    grid_layout->addWidget(password_input, 1, 1);

    top_layout->addLayout(grid_layout);
    authorization_problem->setVisible(false);
    top_layout->addWidget(authorization_problem);

    button_layout->addWidget(login_button);
    top_layout->addLayout(button_layout);

    request->moveToThread(network_thread);

    connect(login_button, &QPushButton::clicked, this, &LoginWindow::startRequest);
    connect(this, &LoginWindow::sendRequest, request.get(), &Request::PostRequest);
    connect(request.get(), &Request::gotHttpData, this, &LoginWindow::onHttpRead);
    connect(request.get(), &Request::httpFinished, this, &LoginWindow::onHttpFinished);
    connect(this, &LoginWindow::UserLogin, static_cast<MainWindow*>(parent), &MainWindow::WriteName);

    network_thread->start();

}

LoginWindow::~LoginWindow(){
    network_thread->quit();
    network_thread->wait();
}

void LoginWindow::startRequest() {
    const QString url_string("http://localhost:8080/api/login_user");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject user_object;
    user_object["name"] = username_input->text();
    user_object["password"] = password_input->text();

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    emit sendRequest(request, body);
}

void LoginWindow::onHttpRead(int status_code, QByteArray data) {

    if (status_code == 200) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject user_object = doc.object();
        User cur_user(
            user_object["id"].toInt()
            , user_object["name"].toString()
            , user_object["password"].toString()
        );
        context_->user_ = cur_user;
        emit UserLogin();
        this->close();
    } else if (status_code == 400) {
        authorization_problem->setVisible(true);
        authorization_problem->setText("Error: invalid login or password");
    }
}

void LoginWindow::onHttpFinished() {
    login_button->setEnabled(true);
}
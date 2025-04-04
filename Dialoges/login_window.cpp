#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>

#include "login_window.h"

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

    setWindowTitle(tr("Investments"));
    resize(400,400);
    
    auto top_layout = new QVBoxLayout(this);
    auto username_layout = new QHBoxLayout();
    auto password_layout = new QHBoxLayout();
    auto problem_layout = new QHBoxLayout();
    auto button_layout = new QHBoxLayout();

    auto username_label = new QLabel("Login: ");
    username_label->setObjectName("username_label");
    username_layout->addWidget(username_label);

    username_input->setObjectName("username_input");
    username_layout->addWidget(username_input);

    username_layout->setAlignment(Qt::AlignLeft);
    top_layout->addLayout(username_layout);

    auto password_label = new QLabel("Password: ");
    password_label->setObjectName("password_label");
    password_layout->addWidget(password_label);

    password_input->setObjectName("password_input");
    password_input->setEchoMode(QLineEdit::Password);
    password_layout->addWidget(password_input);

    password_layout->setAlignment(Qt::AlignLeft);
    top_layout->addLayout(password_layout);

    authorization_problem->setVisible(false);
    problem_layout->addWidget(authorization_problem);
    problem_layout->setAlignment(Qt::AlignHCenter);
    top_layout->addLayout(problem_layout);

    button_layout->addWidget(login_button);
    button_layout->setAlignment(Qt::AlignHCenter);

    request->moveToThread(network_thread);

    connect(login_button, &QPushButton::clicked, this, &LoginWindow::startRequest);
    connect(this, &LoginWindow::sendRequest, request.get(), &Request::onProcessRequest);
    connect(request.get(), &Request::gotHttpData, this, &LoginWindow::onHttpRead);
    connect(request.get(), &Request::httpFinished, this, &LoginWindow::onHttpFinished);
    connect(this, &LoginWindow::UserLogin, static_cast<MainWindow*>(parent), &MainWindow::WriteName);

    network_thread->start();
    top_layout->addLayout(button_layout);

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

void LoginWindow::onHttpRead(QByteArray data) {
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
}

void LoginWindow::onHttpFinished() {
    
    authorization_problem->setVisible(true);
    authorization_problem->setText("Error: invalid login or password");
    login_button->setEnabled(true);
}
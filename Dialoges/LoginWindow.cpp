#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>
#include <QGridLayout>

#include "LoginWindow.h"

LoginWindow::LoginWindow( std::shared_ptr<Context> context, QWidget *parent): 
    QWidget(parent) 
    , username_input_(new QLineEdit(this))
    , password_input_(new QLineEdit(this))
    , authorization_problem_(new QLabel(""))
    , login_button_(new QPushButton("Log In", this))
    , network_thread_(new QThread(this))
    , context_(context)
    , request(new Request())
{

    setWindowTitle(tr("Log In"));
    resize(400,200);
    
    auto top_layout = new QVBoxLayout(this);
    auto grid_layout = new QGridLayout();
    auto button_layout = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignVCenter);

    auto username_label = new QLabel("Login: ");
    username_label->setObjectName("username_label");
    grid_layout->addWidget(username_label, 0, 0);

    username_input_->setObjectName("username_input");
    grid_layout->addWidget(username_input_, 0, 1);

    auto password_label = new QLabel("Password: ");
    password_label->setObjectName("password_label");
    grid_layout->addWidget(password_label, 1, 0);

    password_input_->setObjectName("password_input");
    password_input_->setEchoMode(QLineEdit::Password);
    grid_layout->addWidget(password_input_, 1, 1);

    top_layout->addLayout(grid_layout);

    auto problem_layout = new QHBoxLayout();
    authorization_problem_->setVisible(false);
    authorization_problem_->setObjectName("authorization_problem");
    problem_layout->addWidget(authorization_problem_);
    problem_layout->setAlignment(Qt::AlignHCenter);
    top_layout->addLayout(problem_layout);

    login_button_->setObjectName("auth_user_button");
    button_layout->addWidget(login_button_);
    top_layout->addLayout(button_layout);

    request->moveToThread(network_thread_);

    connect(login_button_, &QPushButton::clicked, this, &LoginWindow::LoginButtonClicked);
    connect(this, &LoginWindow::LoginRequest, request.get(), &Request::PostRequest);
    connect(request.get(), &Request::gotHttpData, this, &LoginWindow::OnHttpRead);
    connect(request.get(), &Request::httpFinished, this, &LoginWindow::onHttpFinished);
    connect(this, &LoginWindow::UserLogin, static_cast<MainWindow*>(parent), &MainWindow::UpdateMainWindow);

    network_thread_->start();

}

LoginWindow::~LoginWindow(){
    network_thread_->quit();
    network_thread_->wait();
}

void LoginWindow::StartRequest() {
    const QString url_string("http://localhost:8080/api/login_user");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject user_object;
    user_object["name"] = username_input_->text();
    user_object["password"] = password_input_->text();

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    emit LoginRequest(request, body);
}

void LoginWindow::OnHttpRead(int status_code, QByteArray data) {

    if (status_code == 200) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject user_object = doc.object();
        User cur_user(
            user_object["id"].toInt()
            , user_object["name"].toString()
            , user_object["password"].toString()
            , user_object["balance"].toInt()
        );
        context_->user_ = cur_user;
        context_->authorized_ = true;
        emit UserLogin();
        this->close();
    } else if (status_code == 400) {
        authorization_problem_->setVisible(true);
        authorization_problem_->setText("Error: invalid login or password");
    }
}

void LoginWindow::onHttpFinished() {
    login_button_->setEnabled(true);
}

void LoginWindow::LoginButtonClicked(){
    if (username_input_->text().isEmpty()) {
        authorization_problem_->setVisible(true);
        authorization_problem_->setText("Please, write login");
        return;
    }
    if (password_input_->text().isEmpty()) {
        authorization_problem_->setVisible(true);
        authorization_problem_->setText("Please, write password");
        return;
    }
    StartRequest();
}
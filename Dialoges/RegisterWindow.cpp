#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

#include "RegisterWindow.h"
#include "../MainWindow.h"

RegisterWindow::RegisterWindow(std::shared_ptr<Context> context, QWidget *parent):
    QDialog(parent) 
    , username_input_(new QLineEdit(this))
    , password_input_(new QLineEdit(this))
    , repeat_pass_input_(new QLineEdit(this))
    , authorization_problem_(new QLabel(""))
    , register_button_(new QPushButton("Register", this))
    , network_thread_(new QThread(this))
    , context_(context)
    , request_(new Request())
{

    setWindowTitle(tr("Registration"));
    resize(400,200);

    auto top_layout = new QVBoxLayout(this);
    auto grid_layout = new QGridLayout();
    auto button_layout = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

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

    auto repeat_pass_label = new QLabel("Confirm Password");
    repeat_pass_label->setObjectName("confirm_pass_label");
    grid_layout->addWidget(repeat_pass_label, 2, 0);

    repeat_pass_input_->setEchoMode(QLineEdit::Password);
    repeat_pass_input_->setObjectName("password_input");
    grid_layout->addWidget(repeat_pass_input_, 2, 1);

    authorization_problem_->setVisible(false);

    button_layout->addWidget(register_button_);

    request_->moveToThread(network_thread_);

    connect(register_button_, &QPushButton::clicked, this, &RegisterWindow::StartRequest);
    connect(this, &RegisterWindow::RegisterRequest, request_.get(), &Request::PostRequest);
    connect(request_.get(), &Request::gotHttpData, this, &RegisterWindow::onHttpRead);
    connect(request_.get(), &Request::httpFinished, this, &RegisterWindow::onHttpFinished);
    connect(this, &RegisterWindow::UserLogin, static_cast<MainWindow*>(parent), &MainWindow::WriteName);

    network_thread_->start();
    top_layout->addLayout(grid_layout);
    top_layout->addWidget(authorization_problem_);
    top_layout->addLayout(button_layout);
}

RegisterWindow::~RegisterWindow(){
    network_thread_->quit();
    network_thread_->wait();
}

void RegisterWindow::StartRequest() {

    if (password_input_->text() != repeat_pass_input_->text()) {
        authorization_problem_->setText("Passwords are different");
        authorization_problem_->setVisible(true);
        return;
    }

    if (password_input_->text().isEmpty()) {
        authorization_problem_->setText("Password is empty");
        authorization_problem_->setVisible(true);
        return;
    }
    const QString url_string("http://localhost:8080/api/register");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject user_object;
    user_object["name"] = username_input_->text();
    user_object["password"] = password_input_->text();

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    emit RegisterRequest(request, body);
}

void RegisterWindow::onHttpRead(int status_code, QByteArray data) {

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
        authorization_problem_->setVisible(true);
        authorization_problem_->setText("Error: user already exists");
    }
}

void RegisterWindow::onHttpFinished() {
    register_button_->setEnabled(true);
}
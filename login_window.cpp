#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include "login_window.h"

LoginWindow::LoginWindow(QWidget *parent): 
    QDialog(parent) 
    , username_input(new QLineEdit(this))
    , password_input(new QLineEdit(this))
    , login_button(new QPushButton("Log In", this))
    , network_thread(new QThread(this))
    , request(new Request())
{

    setWindowTitle(tr("Investments"));
    resize(400,400);

    auto top_layout = new QVBoxLayout(this);
    auto username_layout = new QHBoxLayout();
    auto password_layout = new QHBoxLayout();
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

    button_layout->addWidget(login_button);

    request->moveToThread(network_thread);

    connect(login_button, &QPushButton::clicked, this, &LoginWindow::startRequest);
    connect(this, &LoginWindow::sendRequest, request.get(), &Request::onProcessRequest);
    connect(request.get(), &Request::gotHttpData, this, &LoginWindow::onHttpRead);
    connect(request.get(), &Request::httpFinished, this, &LoginWindow::onHttpFinished);

    network_thread->start();
    // button_layout->setAlignment(Qt::AlignCenter);
    top_layout->addLayout(button_layout);

}

LoginWindow::~LoginWindow(){
    network_thread->quit();
    network_thread->wait();
}

void LoginWindow::startRequest() {
    const QString url_string("http://localhost:8080/api/login_user");
    
    const QUrl url = QUrl::fromUserInput(url_string);
    if (!url.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                    tr("Invalid URL: %1: %2").arg(url_string, url.errorString()));
        return;
    }

    login_button->setEnabled(false);

    emit sendRequest(url);
}

void LoginWindow::onHttpRead(QByteArray data) {
    QMessageBox::information(this, tr("Everything is ok"), tr("Ok"));
}

void LoginWindow::onHttpFinished() {
    login_button->setEnabled(true);
}
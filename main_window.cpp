#include <QVBoxLayout>
#include <QPushButton>

#include "main_window.h"
#include "Dialoges/login_window.h"

MainWindow::MainWindow(QWidget* root): 
    context_(new Context())
    , username(new QLabel())
    , QWidget(root)
{


    setWindowTitle(tr("Investments Application"));
    resize(600,400);

    auto top_layout = new QGridLayout(this);
    auto menu_layout_left = new QHBoxLayout();
    auto menu_layout_right = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignTop);
    menu_layout_left->setAlignment(Qt::AlignLeft);
    menu_layout_right->setAlignment(Qt::AlignRight);
    
    auto login_button = new QPushButton("Log In");
    connect(login_button, &QPushButton::clicked, this, &MainWindow::ClickLogIntButton);
    menu_layout_left->addWidget(login_button);

    auto register_button = new QPushButton("Register");
    connect(register_button, &QPushButton::clicked, this, &MainWindow::ClickRegistrationButton);
    menu_layout_left->addWidget(register_button);

    auto list_stocks_button = new QPushButton("List");
    menu_layout_left->addWidget(list_stocks_button);

    username->setAlignment(Qt::AlignRight);
    menu_layout_right->addWidget(username);
    
    top_layout->addLayout(menu_layout_left, 0, 0);
    top_layout->addLayout(menu_layout_right, 0, 1);

}

void MainWindow::ClickLogIntButton(){

    LoginWindow login_window(context_, this);
    login_window.setModal(false);
    login_window.exec();
}

void MainWindow::ClickRegistrationButton(){
}

void MainWindow::WriteName(){
    username->setText(context_->user_.name_);
}
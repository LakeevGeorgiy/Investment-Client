#include <QVBoxLayout>
#include <QPushButton>

#include "main_window.h"
#include "login_window.h"

MainWindow::MainWindow(QWidget* root): QWidget(root){


    setWindowTitle(tr("Investments Application"));
    resize(600,400);

    auto top_layout = new QVBoxLayout(this);
    auto menu_layout = new QHBoxLayout();
    
    auto login_button = new QPushButton("Log In");
    connect(login_button, &QPushButton::clicked, this, &MainWindow::ClickLogIntButton);
    menu_layout->addWidget(login_button);

    auto list_stocks_button = new QPushButton("List");
    menu_layout->addWidget(list_stocks_button);
    
    top_layout->addLayout(menu_layout);


}

void MainWindow::ClickLogIntButton(){

    LoginWindow login_window(this);
    login_window.setModal(false);
    login_window.exec();
}

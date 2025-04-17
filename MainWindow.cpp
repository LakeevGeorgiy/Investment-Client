#include <QVBoxLayout>
#include <QPushButton>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QDebug>
#include <QJsonValue>
#include <QMessageBox>

#include "MainWindow.h"
#include "Dialoges/LoginWindow.h"
#include "Dialoges/RegisterWindow.h"
#include "Models/Stock.h"

enum class Pages {
    LOGIN,
    REGISTER,
    USER_STOCKS,
    ALL_STOCKS
};

MainWindow::MainWindow(QWidget *root) : 
    context_(new Context())
    , user_stocks_page_(std::make_shared<UserStocksPage>(context_, this))
    , exchange_glass_page_(std::make_shared<ExchangeGlassPage>(context_, this))
    , top_layout_(new QVBoxLayout(this))
    , stacked_widget_(new QStackedWidget())
    , user_stocks_button_(new QPushButton("List"))
    , stocks_layout_(new QTableWidget(0, 4))
    , QWidget(root)
{

    setWindowTitle(tr("Investments Application"));
    resize(920,800);

    this->setMinimumSize(900, 350);

    auto grid_layout = new QVBoxLayout();
    auto menu_layout = new QHBoxLayout();

    top_layout_->setAlignment(Qt::AlignVCenter);
    menu_layout->setAlignment(Qt::AlignHCenter);
    
    auto login_button = new QPushButton("Log In");
    login_button->setObjectName("menu_button");
    connect(login_button, &QPushButton::clicked, this, &MainWindow::ClickLogIntButton);
    menu_layout->addStretch(1);
    menu_layout->addWidget(login_button);
    menu_layout->addStretch(1);

    auto register_button = new QPushButton("Register");
    register_button->setObjectName("menu_button");
    connect(register_button, &QPushButton::clicked, this, &MainWindow::ClickRegistrationButton);
    menu_layout->addWidget(register_button);
    menu_layout->addStretch(1);

    // user_stocks_button_->setVisible(false);
    user_stocks_button_->setObjectName("menu_button");
    connect(user_stocks_button_, &QPushButton::clicked, this, &MainWindow::ClickUserStocksButton);

    auto exchange_button = new QPushButton("Exchange");
    exchange_button->setObjectName("menu_button");
    connect(exchange_button, &QPushButton::clicked, this, &MainWindow::ClickAllStocksButton);

    connect(this, &MainWindow::OpenUserStockPage, user_stocks_page_.get(), &UserStocksPage::OpenPage);
    connect(this, &MainWindow::OpenExchangeGlassPage, exchange_glass_page_.get(), &ExchangeGlassPage::OpenPage);

    menu_layout->addWidget(user_stocks_button_);
    menu_layout->addStretch(1);
    menu_layout->addWidget(exchange_button);
    menu_layout->addStretch(1);
    
    grid_layout->addLayout(menu_layout);
    top_layout_->addLayout(grid_layout);

    auto login_page = new LoginWindow(context_, this);
    auto register_page = new RegisterWindow(context_, this);

    stacked_widget_->addWidget(login_page);
    stacked_widget_->addWidget(register_page);
    stacked_widget_->addWidget(user_stocks_page_.get());
    stacked_widget_->addWidget(exchange_glass_page_.get());
    top_layout_->addWidget(stacked_widget_);

    stocks_layout_->setVisible(false);
    top_layout_->addWidget(stocks_layout_);

}

void MainWindow::UpdateMainWindow(){
    user_stocks_button_->setVisible(true);
}

void MainWindow::ClickLogIntButton(){
    stacked_widget_->setCurrentIndex((int)Pages::LOGIN);
}

void MainWindow::ClickRegistrationButton(){
    stacked_widget_->setCurrentIndex((int)Pages::REGISTER);
}

void MainWindow::ClickUserStocksButton(){
    emit OpenUserStockPage();
    stacked_widget_->setCurrentIndex((int)Pages::USER_STOCKS);
}

void MainWindow::ClickAllStocksButton(){
    emit OpenExchangeGlassPage();
    stacked_widget_->setCurrentIndex((int)Pages::ALL_STOCKS);
}

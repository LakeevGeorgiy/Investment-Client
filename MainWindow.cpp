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

MainWindow::MainWindow(QWidget *root) : 
    context_(new Context())
    , username_(new QLabel())
    , top_layout_(new QVBoxLayout(this))
    , stocks_layout_(new QTableWidget(0, 4))
    , user_stocks_request_(new Request())
    , sell_stocks_request(new Request())
    , QWidget(root)
{

    setWindowTitle(tr("Investments Application"));
    resize(600,400);

    auto grid_layout = new QGridLayout();
    auto menu_layout_left = new QHBoxLayout();
    auto menu_layout_right = new QHBoxLayout();

    top_layout_->setAlignment(Qt::AlignTop);
    menu_layout_left->setAlignment(Qt::AlignLeft);
    menu_layout_right->setAlignment(Qt::AlignRight);
    
    auto login_button = new QPushButton("Log In");
    connect(login_button, &QPushButton::clicked, this, &MainWindow::ClickLogIntButton);
    menu_layout_left->addWidget(login_button);

    auto register_button = new QPushButton("Register");
    connect(register_button, &QPushButton::clicked, this, &MainWindow::ClickRegistrationButton);
    menu_layout_left->addWidget(register_button);

    auto user_stocks_button = new QPushButton("List");
    connect(user_stocks_button, &QPushButton::clicked, this, &MainWindow::UserStocksRequest);

    auto exchange_button = new QPushButton("Exchange");
    // connect(exchange_button, &QPushButton::clicked, this, &MainWindow::ClickExchangeButton);

    connect(this, &MainWindow::SendUserStockRequest, user_stocks_request_.get(), &Request::GetRequest);
    connect(this, &MainWindow::SendSellStockRequest, sell_stocks_request.get(), &Request::PostRequest);

    connect(user_stocks_request_.get(), &Request::gotHttpData, this, &MainWindow::UserStocksRead);
    connect(sell_stocks_request.get(), &Request::gotHttpData, this, &MainWindow::UserSellStocksRead);
    // connect(user_stocks_request_.get(), &Request::httpFinished, this, &MainWindow::onHttpFinished);
    // connect(sell_stocks_request.get(), &Request::httpFinished, this, &MainWindow::onHttpFinished);

    menu_layout_left->addWidget(user_stocks_button);

    username_->setAlignment(Qt::AlignRight);
    menu_layout_right->addWidget(username_);
    
    grid_layout->addLayout(menu_layout_left, 0, 0);
    grid_layout->addLayout(menu_layout_right, 0, 2);
    top_layout_->addLayout(grid_layout);

    stocks_layout_->setVisible(false);
    top_layout_->addWidget(stocks_layout_);

}

void MainWindow::WriteName(){
    username_->setText(context_->user_.name_);
}

void MainWindow::UserStocksRequest() {

    const QString url_string("http://localhost:8080/api/list_user_stock");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    int id = context_->user_.id_;
    QJsonObject user_object;
    user_object["user_id"] = QJsonValue(id);

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    emit SendUserStockRequest(request, body);
}

void MainWindow::UserStocksRead(int status_code, QByteArray data) {

    if (status_code == 200) {

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray stocks_json = doc.array();

        if (stocks_json.empty()) {
            QMessageBox message(this);
            message.setInformativeText("You don't have stocks");
            message.exec();
        }

        stocks_layout_->clear();
        stocks_layout_->setRowCount(stocks_json.size());

        for (int i = 0; i < stocks_json.size(); ++i) {

            auto stock_object = QJsonValue(stocks_json[i]);

            auto stock = Stock(
                stock_object["id"].toInt(),
                stock_object["cost"].toInt(),
                stock_object["count"].toInt(),
                stock_object["company_name"].toString()
            );

            qDebug() << stock.company_name_;

            auto cost = new QTableWidgetItem(QString::number(stock.cost_));
            auto count = new QTableWidgetItem(QString::number(stock.count_));
            auto cell_button = new QPushButton("Sell");
            auto company = new QTableWidgetItem(stock.company_name_);
            connect(cell_button, &QPushButton::clicked, [this, stock]() {
                this->UserSellRequest(stock);
                this->UserStocksRequest();
            });
        
            // stocks_layout_->insertRow(i);
        
            stocks_layout_->setItem(i, 0, cost);
            stocks_layout_->setItem(i, 1, count);
            stocks_layout_->setItem(i, 2, company);
            stocks_layout_->setCellWidget(i, 3, cell_button);
        }
        stocks_layout_->setVisible(true);
    } 
}

void MainWindow::UserSellStocksRead(int status_code, QByteArray data){
    if (status_code == 200) {
        qDebug() << "sell stocks\n";
    } else {
        qDebug() << "can't sell stocks\n";
    }
}

void MainWindow::UserSellRequest(const Stock &stock){

    const QString url_string("http://localhost:8080/api/sell_stocks");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject user_object;
    user_object["user_id"] = QJsonValue((int)context_->user_.id_);
    user_object["stock_id"] = QJsonValue((int)stock.id_);
    user_object["count"] = QJsonValue((int)stock.count_);

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    qDebug() << "sell stock: " << stock.id_;

    emit SendSellStockRequest(request, body);
}

void MainWindow::ClickLogIntButton()
{
    LoginWindow login_window(context_, this);
    login_window.setModal(false);
    login_window.exec();
}

void MainWindow::ClickRegistrationButton(){
    RegisterWindow register_window(context_, this);
    register_window.setModal(false);
    register_window.exec();
}
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QMessageBox>
#include <QHeaderView>

#include "UserStocksPage.h"
#include "../Dialoges/SellStocksWindow.h"

UserStocksPage::UserStocksPage(std::shared_ptr<Context> context, QWidget *root):
    stocks_grid_(new QTableWidget(0, 4))
    , balance_(new QLabel())
    , context_(context)
    , user_stocks_request_(new Request())
    , network_thread_(new QThread())
    , QWidget(root)
{
    auto top_layout = new QVBoxLayout(this);
    auto balance_layout = new QHBoxLayout();
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
    balance_layout->addWidget(balance_);
    top_layout->addLayout(balance_layout);

    auto grid_layout = new QHBoxLayout();
    grid_layout->addWidget(stocks_grid_);
    grid_layout->setAlignment(Qt::AlignHCenter);
    stocks_grid_->resizeRowsToContents();
    top_layout->addLayout(grid_layout);

    user_stocks_request_->moveToThread(network_thread_);
    connect(this, &UserStocksPage::SendUserStockRequest, user_stocks_request_.get(), &Request::GetRequest);
    connect(user_stocks_request_.get(), &Request::gotHttpData, this, &UserStocksPage::UserStocksRead);
    network_thread_->start();
}

UserStocksPage::~UserStocksPage(){
    network_thread_->quit();
    network_thread_->wait();
}

void UserStocksPage::WriteBalance(){
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
}

void UserStocksPage::OpenPage() {
    WriteBalance();
    UserStocksRequest();
}

void UserStocksPage::UserStocksRequest(){

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

void UserStocksPage::UserStocksRead(int status_code, QByteArray data) {

    if (status_code == 200) {

        qDebug() << "read status code";

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray stocks_json = doc.array();

        if (stocks_json.empty()) {
            QMessageBox message(this);
            message.setFixedSize(200, 50);
            message.setInformativeText("You don't have stocks");
            message.exec();
        }

        std::function button_click = [this](Stock stock) {
            SellStocksWindow sell_dialoge(stock, context_, this);
            sell_dialoge.setModal(false);
            sell_dialoge.exec();
        };
        PrintStocks(stocks_json, "Sell", button_click);
    } 
}

void UserStocksPage::PrintStocks(QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click){

    stocks_grid_->clear();
    stocks_grid_->setRowCount(stocks_json.size());
    stocks_grid_->setHorizontalHeaderLabels({"Cost", "Count", "Company", ""});
    stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    stocks_grid_->verticalHeader()->setVisible(false);

    for (int i = 0; i < stocks_json.size(); ++i) {

        auto stock_object = QJsonValue(stocks_json[i]);

        Stock stock(
            stock_object["id"].toInt(),
            stock_object["cost"].toInt(),
            stock_object["count"].toInt(),
            stock_object["company_name"].toString(),
            stock_object["image_url"].toString()
        );

        qDebug() << "count: " << stock.count_;
        
        if (stock.count_ == 0) {
            continue;
        }

        auto cost = new QTableWidgetItem(QString::number(stock.cost_));
        cost->setTextAlignment(Qt::AlignHCenter);
        auto count = new QTableWidgetItem(QString::number(stock.count_));
        count->setTextAlignment(Qt::AlignHCenter);
        auto cell_button = new QPushButton(button_text);
        auto company = new QTableWidgetItem(stock.company_name_);
        company->setTextAlignment(Qt::AlignHCenter); 
        connect(cell_button, &QPushButton::clicked, std::bind(button_click, stock));
    
    
        stocks_grid_->setItem(i, 0, cost);
        stocks_grid_->setItem(i, 1, count);
        stocks_grid_->setItem(i, 2, company);
        stocks_grid_->setCellWidget(i, 3, cell_button);
    }
    stocks_grid_->setVisible(true);

}
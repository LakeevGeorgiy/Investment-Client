#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>

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
    balance_layout->setAlignment(Qt::AlignHCenter);
    
    balance_->setObjectName("balance");
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
    balance_layout->addWidget(balance_);
    top_layout->addLayout(balance_layout);

    stocks_grid_->setShowGrid(false);
    top_layout->addWidget(stocks_grid_);

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
    balance_->setStyleSheet(
        "QLabel {"
        "   font-size: 17px;"
        "   color: black;"
        "}"
    );
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
}

void UserStocksPage::OpenPage() {
    if (!context_->authorized_) {
        balance_->setObjectName("balance");
        balance_->setStyleSheet(
            "QLabel {"
            "   font-size: 17px;"
            "   color: red;"
            "}"
        );
        balance_->setText("You must be authorized");
        stocks_grid_->setVisible(false);
        return;
    }
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

    size_t cur_col = 0;
    size_t cur_row = 0;
    std::vector<Stock> stocks;

    for (size_t i = 0; i < stocks_json.size(); ++i) {

        auto stock_object = QJsonValue(stocks_json[i]);
        
        auto stock = Stock(
            stock_object["id"].toInt(),
            stock_object["cost"].toInt(),
            stock_object["count"].toInt(),
            stock_object["company_name"].toString(),
            stock_object["image_url"].toString()
        );

        if (stock.count_ == 0) {
            continue;
        }

        stocks.emplace_back(stock);
    }
    
    const QSize card_size(220, 240);
    const QSize rows_cols_number(stocks.size() / 4 + 1, 4);

    stocks_grid_->setVisible(true);
    stocks_grid_->clear();
    stocks_grid_->setColumnCount(rows_cols_number.height());
    stocks_grid_->setRowCount(rows_cols_number.width());
    stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    stocks_grid_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    stocks_grid_->verticalHeader()->setVisible(false);
    stocks_grid_->horizontalHeader()->setVisible(false);

    stocks_grid_->verticalHeader()->setDefaultSectionSize(card_size.width());
    stocks_grid_->horizontalHeader()->setDefaultSectionSize(card_size.height());
    
    for (auto& stock : stocks) {

        auto card = new QFrame();
        card->setObjectName("stock_card");
        card->setFixedSize(card_size);

        auto stock_layout = new QVBoxLayout(card);
        stock_layout->setAlignment(Qt::AlignVCenter);

        auto infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(4);

        auto costLabel = new QLabel(QString("Cost: %1").arg(stock.cost_));
        costLabel->setObjectName("stock_info");
        
        auto countLabel = new QLabel(QString("Available: %1").arg(stock.count_));
        countLabel->setObjectName("stock_info");

        infoLayout->addWidget(costLabel);
        infoLayout->addWidget(countLabel);
        stock_layout->addLayout(infoLayout);
        
        auto cell_button = new QPushButton(stock.company_name_);
        cell_button->setObjectName("sell_button");
        connect(cell_button, &QPushButton::clicked, std::bind(button_click, stock));
        stock_layout->addWidget(cell_button);
        
        stocks_grid_->setCellWidget(cur_row, cur_col, card);
        stocks_grid_->setRowHeight(cur_row, 240);
        stocks_grid_->setColumnWidth(cur_col, 220);

        qDebug() << "row: " << cur_row << " col: " << cur_col;

        ++cur_col;
        cur_row += cur_col / rows_cols_number.height();
        cur_col %= rows_cols_number.height();

    }

}
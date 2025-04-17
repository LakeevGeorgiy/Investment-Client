#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QMessageBox>
#include <QHeaderView>
#include <QIcon>
#include <QFrame>
#include <QBuffer>
#include <QImageReader>
#include <vector>
#include <QWindow>

#include "ExchangeGlassPage.h"
#include "../Dialoges/BuyStocksWindow.h"

ExchangeGlassPage::ExchangeGlassPage(std::shared_ptr<Context> context, QWidget *root):
    top_layout_(new QVBoxLayout(this))
    , stocks_grid_(new QTableWidget(0, 4))
    , balance_(new QLabel())
    , context_(context)
    , all_stocks_request_(new Request())
    , image_request_(new Request())
    , network_thread_(new QThread())
    , QWidget(root)
{
    top_layout_->setAlignment(Qt::AlignTop);

    auto balance_layout = new QHBoxLayout();
    balance_layout->addWidget(balance_);
    balance_layout->setAlignment(Qt::AlignHCenter);
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
    top_layout_->addLayout(balance_layout);

    stocks_grid_->setShowGrid(false);
    top_layout_->addWidget(stocks_grid_);

    all_stocks_request_->moveToThread(network_thread_);
    connect(this, &ExchangeGlassPage::SendAllStocksRequest, all_stocks_request_.get(), &Request::GetRequest);
    connect(all_stocks_request_.get(), &Request::gotHttpData, this, &ExchangeGlassPage::AllStocksRead);

    network_thread_->start();
}

ExchangeGlassPage::~ExchangeGlassPage(){
    network_thread_->quit();
    network_thread_->wait();
}

void ExchangeGlassPage::WriteBalance(){
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
}

void ExchangeGlassPage::OpenPage() {
    WriteBalance();
    AllStocksRequest();
}

void ExchangeGlassPage::AllStocksRequest(){

    const QString url_string("http://localhost:8080/api/list_all_stocks");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    QByteArray body;

    emit SendAllStocksRequest(request, body);
}

void ExchangeGlassPage::AllStocksRead(int status_code, QByteArray data) {

    if (status_code == 200) {

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray stocks_json = doc.array();

        if (stocks_json.empty()) {
            QMessageBox message(this);
            message.setInformativeText("No stocks are not available");
            message.exec();
        }

        std::function button_click = [this](Stock stock) {
            BuyStocksWindow buy_dialog(stock, context_, this);
            buy_dialog.setModal(false);
            buy_dialog.exec();
        };
        qDebug() << "print stocks\n";
        PrintStocks(stocks_json, "Buy", button_click);
    }
}

QSize ExchangeGlassPage::NumberOfRowsAndCols(size_t stocks_number) {
    const QSize size_of_card(220, 240);
    QSize window_size = this->size();

    size_t cols_number = window_size.width() / size_of_card.width();
    size_t rows_number = stocks_number / cols_number + 1;
    return QSize(rows_number, cols_number);
}

void ExchangeGlassPage::PrintStocks(QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click){

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

    stocks_grid_->clear();
    stocks_grid_->setColumnCount(rows_cols_number.height());
    stocks_grid_->setRowCount(rows_cols_number.width());
    // stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int col = 0; col + 1< rows_cols_number.height(); ++col) {
        stocks_grid_->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
        // stocks_grid_->horizontalHeader()->setStretchLastSection(false);
    }
    stocks_grid_->horizontalHeader()->setStretchLastSection(false); 
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
        
        stocks_grid_->setCellWidget(cur_row, cur_col, card);
        stock_layout->addWidget(cell_button);
        stocks_grid_->setRowHeight(cur_row, 240);
        stocks_grid_->setColumnWidth(cur_col, 220);

        qDebug() << "row: " << cur_row << " col: " << cur_col;

        ++cur_col;
        cur_row += cur_col / rows_cols_number.height();
        cur_col %= rows_cols_number.height();

        if (cur_col > 0) {
        }
    }

}
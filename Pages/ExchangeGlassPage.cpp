#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QMessageBox>
#include <QHeaderView>
#include <QIcon>

#include "ExchangeGlassPage.h"
#include "../Dialoges/BuyStocksWindow.h"

ExchangeGlassPage::ExchangeGlassPage(std::shared_ptr<Context> context, QWidget *root):
    stocks_grid_(new QTableWidget(0, 4))
    , balance_(new QLabel())
    , context_(context)
    , all_stocks_request_(new Request())
    , network_thread_(new QThread())
    , QWidget(root)
{
    auto top_layout = new QVBoxLayout(this);

    auto balance_layout = new QHBoxLayout();
    balance_layout->addWidget(balance_);
    balance_layout->setAlignment(Qt::AlignHCenter);
    balance_->setText("Your current balance: " + QString::number(context_->user_.balance_));
    top_layout->addLayout(balance_layout);

    stocks_grid_->resizeRowsToContents();
    // top_layout->addLayout(grid_layout);
    top_layout->addWidget(stocks_grid_);

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

void ExchangeGlassPage::PrintStocks(QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click){

    stocks_grid_->clear();
    stocks_grid_->setRowCount(stocks_json.size());
    stocks_grid_->setHorizontalHeaderLabels({"Cost", "Count", "Company", ""});
    stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    stocks_grid_->verticalHeader()->setVisible(false);
    stocks_grid_->setWordWrap(true);

    for (int i = 0; i < stocks_json.size(); ++i) {

        auto stock_object = QJsonValue(stocks_json[i]);

        auto stock = Stock(
            stock_object["id"].toInt(),
            stock_object["cost"].toInt(),
            stock_object["count"].toInt(),
            stock_object["company_name"].toString()
        );

        auto cost = new QTableWidgetItem(QString::number(stock.cost_));
        cost->setTextAlignment(Qt::AlignHCenter);
        auto count = new QTableWidgetItem(QString::number(stock.count_));
        count->setTextAlignment(Qt::AlignHCenter);
        auto cell_button = new QPushButton();
        cell_button->setObjectName("sell_button");
        QIcon icon("../resources/ozon.jpg");
        cell_button->setIcon(icon);
        cell_button->setIconSize(QSize(300, 300));
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

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

    connect(this, &ExchangeGlassPage::SendImageRequest, image_request_.get(), &Request::GetRequest);
    // connect(image_request_.get(), &Request::gotHttpData, this, &ExchangeGlassPage)

    connect(this->windowHandle(), &QWindow::widthChanged, this, &ExchangeGlassPage::AllStocksRequest);
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

void ExchangeGlassPage::ImageRequest(QString url_string) {
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    QByteArray body;

    emit SendImageRequest(request, body);
}

void ExchangeGlassPage::AllStocksRequest(){

    const QString url_string("http://localhost:8080/api/list_all_stocks");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    QByteArray body;

    emit SendAllStocksRequest(request, body);
}

void ExchangeGlassPage::ImageRead(QPushButton* button, int status_code, QByteArray data){
    if (status_code != 200) {
        return;
    }

    QBuffer buffer(&const_cast<QByteArray&>(data));
    buffer.open(QIODevice::ReadOnly);
    
    QImageReader reader(&buffer, "JPG");
    if (!reader.canRead()) {
        qWarning() << "Cannot read JPG data:" << reader.errorString();
        return;
    }
    
    QImage image = reader.read();
    if (image.isNull()) {
        qWarning() << "Failed to read JPG image:" << reader.errorString();
        return;
    }
    
    QPixmap pixmap = QPixmap::fromImage(image);

    QSize icon_size(200, 200);
    QIcon icon("../resources/ozon.jpg");

    if (pixmap.loadFromData(data, "JPG")) {
        icon = QIcon(pixmap);
        button->setIcon(icon);
        return;
    }
    qDebug() << "image read\n";

    button->setIcon(icon);
    button->setIconSize(icon_size);
    button->setFlat(true);

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
    
    QSize table_size = NumberOfRowsAndCols(stocks.size());
    qDebug() << "table size: " << table_size;   

    stocks_grid_->clear();
    stocks_grid_->setColumnCount(table_size.height());
    stocks_grid_->setRowCount(table_size.width());
    stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    stocks_grid_->verticalHeader()->setVisible(false);
    stocks_grid_->horizontalHeader()->setVisible(false);

    stocks_grid_->verticalHeader()->setDefaultSectionSize(240);
    // stocks_grid_->verticalHeader()->setSectionResizeMode(QHeaderView::);

    stocks_grid_->horizontalHeader()->setDefaultSectionSize(220);
    // stocks_grid_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    
    for (auto& stock : stocks) {

        auto card = new QFrame();
        card->setObjectName("stock_card");
        card->setFixedSize(QSize(220, 240));

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
        cur_row += cur_col / table_size.height();
        cur_col %= table_size.height();

    }

    top_layout_->addWidget(stocks_grid_);

}



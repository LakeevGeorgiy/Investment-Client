#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>

#include "BuyStocksWindow.h"
#include "../MainWindow.h"

BuyStocksWindow::BuyStocksWindow(const Stock& stock, std::shared_ptr<Context> context, QWidget *parent) :
    cnt_input_(new QLineEdit())
    , buy_problem_(new QLabel())
    , buy_button_(new QPushButton("Buy"))
    , network_thread_(new QThread())
    , stock_(stock)
    , context_(context)
    , request_(new Request())
    , QDialog(parent)
{

    setWindowTitle("Buy stock: " + stock_.company_name_);
    resize(300,100);
    
    auto top_layout = new QVBoxLayout(this);
    auto grid_layout = new QGridLayout();
    auto button_layout = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    auto cnt_label = new QLabel("Count: ");
    cnt_label->setObjectName("username_label");
    grid_layout->addWidget(cnt_label, 0, 0);

    cnt_input_->setObjectName("buy_cnt_input");
    auto validator = new QIntValidator(cnt_input_);
    cnt_input_->setValidator(validator);
    grid_layout->addWidget(cnt_input_, 0, 1);

    top_layout->addLayout(grid_layout);
    buy_problem_->setVisible(false);
    buy_problem_->setObjectName("buy_problem");
    top_layout->addWidget(buy_problem_);

    button_layout->addWidget(buy_button_);
    top_layout->addLayout(button_layout);

    request_->moveToThread(network_thread_);

    connect(buy_button_, &QPushButton::clicked, this, &BuyStocksWindow::BuyButtonClicked);
    connect(this, &BuyStocksWindow::BuyRequest, request_.get(), &Request::PostRequest);
    connect(request_.get(), &Request::gotHttpData, this, &BuyStocksWindow::onHttpRead);
    connect(this, &BuyStocksWindow::SuccessBuy, static_cast<ExchangeGlassPage*>(parent), &ExchangeGlassPage::OpenPage);
    connect(this, &BuyStocksWindow::UpdateBalance, static_cast<ExchangeGlassPage*>(parent), &ExchangeGlassPage::WriteBalance);

    network_thread_->start();

}

BuyStocksWindow::~BuyStocksWindow() {
    network_thread_->quit();
    network_thread_->wait();
}

void BuyStocksWindow::StartRequest(){

    const QString url_string("http://localhost:8080/api/buy_stocks");
    const QUrl url = QUrl::fromUserInput(url_string);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    int count = cnt_input_->text().toInt();
    QJsonObject user_object;
    user_object["user_id"] = QJsonValue((int)context_->user_.id_);
    user_object["stock_id"] = QJsonValue((int)stock_.id_);
    user_object["count"] = QJsonValue(count);

    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());

    emit BuyRequest(request, body);
}

void BuyStocksWindow::onHttpRead(int status_code, QByteArray data){

    if (status_code != 200) {
        buy_problem_->setVisible(true);
        buy_problem_->setText(data);
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject user_object = doc.object();
        context_->user_.balance_ = user_object["balance"].toInt();
        
        emit SuccessBuy();
        emit UpdateBalance();
        this->close();
    }

}

void BuyStocksWindow::BuyButtonClicked(){

    if (!context_->authorized_) {
        buy_problem_->setText("To buy you need to log in");
        buy_problem_->setVisible(true);
        return;        
    }
    if (cnt_input_->text().isEmpty()) {
        buy_problem_->setText("Write number of stocks");
        buy_problem_->setVisible(true);
        return;
    }
    if (cnt_input_->text().toInt() <= 0) {
        buy_problem_->setText("Value must be greater than zero");
        buy_problem_->setVisible(true);
        return;
    }
    StartRequest();
}
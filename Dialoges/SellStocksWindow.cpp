#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>

#include "SellStocksWindow.h"
#include "../Pages/UserStocksPage.h"

SellStocksWindow::SellStocksWindow(const Stock& stock, std::shared_ptr<Context> context, QWidget *parent) :
    cnt_input_(new QLineEdit())
    , sell_problem_(new QLabel())
    , sell_button_(new QPushButton("Sell"))
    , network_thread_(new QThread())
    , stock_(stock)
    , context_(context)
    , request_(new Request())
    , QDialog(parent)
{

    setWindowTitle("Sell stock: " + stock_.company_name_);
    resize(300,100);
    
    auto top_layout = new QVBoxLayout(this);
    auto grid_layout = new QGridLayout();
    auto button_layout = new QHBoxLayout();

    top_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    auto cnt_label = new QLabel("Count: ");
    cnt_label->setObjectName("username_label");
    grid_layout->addWidget(cnt_label, 0, 0);

    cnt_input_->setObjectName("sell_cnt_input");
    auto validator = new QIntValidator(cnt_input_);
    cnt_input_->setValidator(validator);
    grid_layout->addWidget(cnt_input_, 0, 1);

    top_layout->addLayout(grid_layout);
    sell_problem_->setVisible(false);
    sell_problem_->setObjectName("sell_problem");

    top_layout->addWidget(sell_problem_);

    button_layout->addWidget(sell_button_);
    top_layout->addLayout(button_layout);

    request_->moveToThread(network_thread_);

    connect(sell_button_, &QPushButton::clicked, this, &SellStocksWindow::SellButtonClicked);
    connect(this, &SellStocksWindow::SellRequest, request_.get(), &Request::PostRequest);
    connect(request_.get(), &Request::gotHttpData, this, &SellStocksWindow::onHttpRead);
    // connect(request_.get(), &Request::httpFinished, this, &SellStocksWindow::onHttpFinished);
    connect(this, &SellStocksWindow::SuccessSell, static_cast<UserStocksPage*>(parent), &UserStocksPage::OpenPage);
    connect(this, &SellStocksWindow::UpdateBalance, static_cast<UserStocksPage*>(parent), &UserStocksPage::WriteBalance);

    network_thread_->start();

}

SellStocksWindow::~SellStocksWindow() {
    network_thread_->quit();
    network_thread_->wait();
}

void SellStocksWindow::StartRequest(){

    const QString url_string("http://localhost:8080/api/sell_stocks");
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

    // qDebug() << "sell stock: " << stock_.id_;

    emit SellRequest(request, body);
}

void SellStocksWindow::onHttpRead(int status_code, QByteArray data){

    if (status_code != 200) {
        sell_problem_->setVisible(true);
        sell_problem_->setText(data);
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject user_object = doc.object();
        context_->user_.balance_ = user_object["balance"].toInt();
        
        emit SuccessSell();
        emit UpdateBalance();
        this->close();
    }

}

void SellStocksWindow::SellButtonClicked(){
    if (!context_->authorized_) {
        sell_problem_->setText("To buy you need to log in");
        sell_problem_->setVisible(true);
        return;        
    }
    if (cnt_input_->text().isEmpty()) {
        sell_problem_->setText("Write number of stocks");
        sell_problem_->setVisible(true);
        return;
    }
    if (cnt_input_->text().toInt() <= 0) {
        sell_problem_->setText("Value must be greater than zero");
        sell_problem_->setVisible(true);
        return;
    }
    StartRequest();
}
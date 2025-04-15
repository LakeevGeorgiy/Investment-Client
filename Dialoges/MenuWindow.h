#pragma once

#include <QLabel>
#include <memory>
#include <QGridLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QNetworkRequest>

#include "../Context.h"
#include "../Requests/Request.h"
#include "../Models/Stock.h"

class MenuWindow : public QWidget {
private:

    QLabel* balance_;
    QLabel* username_;
    QVBoxLayout* top_layout_;
    QPushButton* user_stocks_button_;
    QTableWidget* stocks_layout_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> user_stocks_request_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> sell_stocks_request;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> all_stocks_request_;

// signals:

//     void SendUserStockRequest(QNetworkRequest request, QByteArray body);
//     void SendAllStocksRequest(QNetworkRequest request, QByteArray body);

public slots:

    void UserStocksRequest();
    void AllStocksRequest();
    void UserStocksRead(int status_code, QByteArray data);
    void AllStocksRead(int status_code, QByteArray data);

public:

    MenuWindow(QWidget* root = nullptr);

private:

    void PrintStocks(
        QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click);
    void ClickLogIntButton();
    void ClickRegistrationButton();
};
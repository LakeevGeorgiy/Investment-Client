#pragma once

#include <QMainWindow>
#include <QLabel>
#include <memory>
#include <QGridLayout>
#include <QTableWidget>
#include <QNetworkRequest>

#include "Context.h"
#include "Requests/Request.h"
#include "Models/Stock.h"

class MainWindow : public QWidget {

    Q_OBJECT
private:

    QLabel* username_;
    QVBoxLayout* top_layout_;
    QTableWidget* stocks_layout_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> user_stocks_request_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> sell_stocks_request;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> all_stocks_request_;

signals:

    void SendUserStockRequest(QNetworkRequest request, QByteArray body);
    void SendSellStockRequest(QNetworkRequest request, QByteArray body);

public slots:

    void WriteName();
    void UserStocksRequest();
    void UserStocksRead(int status_code, QByteArray data);
    void UserSellStocksRead(int status_code, QByteArray data);


public:

    MainWindow(QWidget* root = nullptr);

private:

    void UserSellRequest(const Stock& stock);
    void ClickLogIntButton();
    void ClickRegistrationButton();
};

#pragma once

#include <QTableWidget>
#include <QThread>
#include <QLabel>

#include "../Context.h"
#include "../Requests/Request.h"
#include "../Models/Stock.h"

class UserStocksPage : public QWidget {

    Q_OBJECT

private:

    QLabel* balance_;
    QThread* network_thread_;
    QTableWidget* stocks_grid_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> user_stocks_request_;

public:

    UserStocksPage(std::shared_ptr<Context> context, QWidget* root = nullptr);
    ~UserStocksPage();

signals:

    void SendUserStockRequest(QNetworkRequest request, QByteArray body);

public slots:

    void OpenPage();
    void UserStocksRead(int status_code, QByteArray data);
    void WriteBalance();
    
private:
    
    void UserStocksRequest();
    void PrintStocks(
        QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click);
};
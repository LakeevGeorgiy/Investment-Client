#pragma once

#include <QTableWidget>
#include <QThread>
#include <QLabel>
#include <QTableView>

#include "../Context.h"
#include "../Requests/Request.h"
#include "../Models/Stock.h"

class ExchangeGlassPage : public QWidget {

    Q_OBJECT

private:

    QLabel* balance_;
    QThread* network_thread_;
    QTableWidget* stocks_grid_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> all_stocks_request_;

public:

    ExchangeGlassPage(std::shared_ptr<Context> context, QWidget* root = nullptr);
    ~ExchangeGlassPage();

signals:

    void SendAllStocksRequest(QNetworkRequest request, QByteArray body);

public slots:

    void OpenPage();
    void WriteBalance();
    void AllStocksRead(int status_code, QByteArray data);

private:

    void AllStocksRequest();
    void PrintStocks(
        QJsonArray& stocks_json, const QString& button_text, std::function<void(Stock)>& button_click);
};
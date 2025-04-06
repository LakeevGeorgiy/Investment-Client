#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QThread>

#include "../Context.h"
#include "../Models/Stock.h"
#include "../Requests/Request.h"


class BuyStocksWindow : public QDialog {
    Q_OBJECT

public:

    BuyStocksWindow(const Stock& stock, std::shared_ptr<Context> context, QWidget* parent = nullptr);
    ~BuyStocksWindow();

signals:

    void BuyRequest(QNetworkRequest request, QByteArray body);
    void SuccessBuy();

private slots:

    void StartRequest();
    void onHttpRead(int status_code, QByteArray data);

private:

    QLineEdit* cnt_input_;
    QLabel* buy_problem_;
    QPushButton* buy_button_;
    QThread* network_thread_;

    const Stock stock_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> request_;
};
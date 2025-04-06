#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QThread>

#include "../Context.h"
#include "../Models/Stock.h"
#include "../Requests/Request.h"


class SellStocksWindow : public QDialog {
    Q_OBJECT

public:

    SellStocksWindow(const Stock& stock, std::shared_ptr<Context> context, QWidget* parent = nullptr);
    ~SellStocksWindow();

signals:

    void SellRequest(QNetworkRequest request, QByteArray body);
    void SuccessSell();

private slots:

    void StartRequest();
    void onHttpRead(int status_code, QByteArray data);

private:

    QLineEdit* cnt_input_;
    QLabel* sell_problem_;
    QPushButton* sell_button_;
    QThread* network_thread_;

    const Stock stock_;
    std::shared_ptr<Context> context_;
    QScopedPointer<Request, QScopedPointerDeleter<Request>> request_;
};
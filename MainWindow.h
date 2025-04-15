#pragma once

#include <QMainWindow>
#include <QLabel>
#include <memory>
#include <QGridLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QNetworkRequest>

#include "Context.h"
#include "Requests/Request.h"
#include "Models/Stock.h"
#include "Pages/UserStocksPage.h"
#include "Pages/ExchangeGlassPage.h"

class MainWindow : public QWidget {

    Q_OBJECT
private:

    QVBoxLayout* top_layout_;
    QStackedWidget* stacked_widget_;
    QPushButton* user_stocks_button_;
    QTableWidget* stocks_layout_;
    std::shared_ptr<Context> context_;
    std::shared_ptr<UserStocksPage> user_stocks_page_;
    std::shared_ptr<ExchangeGlassPage> exchange_glass_page_;

signals:

    void OpenUserStockPage();
    void OpenExchangeGlassPage();

public slots:

    void UpdateMainWindow();

public:

    MainWindow(QWidget* root = nullptr);

private:

    void ClickLogIntButton();
    void ClickRegistrationButton();
    void ClickUserStocksButton();
    void ClickAllStocksButton();

};

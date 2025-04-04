#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Request : public QObject{

    Q_OBJECT

public:

    Request();

signals:

    void gotHttpData(QByteArray data);
    void httpFinished();

public slots:

    void onProcessRequest(QNetworkRequest request, QByteArray body);
    void httpReadyRead();

private:

    QUrl base_url;
    QScopedPointer<QNetworkAccessManager, QScopedPointerDeleter<QNetworkAccessManager>> network_manager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleter<QNetworkReply>> reply;

};
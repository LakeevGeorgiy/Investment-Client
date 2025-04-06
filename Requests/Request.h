#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Request : public QObject{

    Q_OBJECT

public:

    Request();

signals:

    void gotHttpData(int status_code, QByteArray data);
    void httpFinished();

public slots:

    void GetRequest(QNetworkRequest request, QByteArray body);
    void PostRequest(QNetworkRequest request, QByteArray body);
    void httpReadyRead();

private:

    QScopedPointer<QNetworkAccessManager, QScopedPointerDeleter<QNetworkAccessManager>> network_manager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleter<QNetworkReply>> reply;

};
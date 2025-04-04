#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Request : public QObject{

    Q_OBJECT

public:

    Request();

signals:

    void gotHttpData(QByteArray data);
    void httpFinished();

public slots:

    void onProcessRequest(QUrl url);
    void httpReadyRead();

private:

    QUrl base_url;
    QScopedPointer<QNetworkAccessManager, QScopedPointerDeleter<QNetworkAccessManager>> network_manager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleter<QNetworkReply>> reply;

};
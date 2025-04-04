#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>

#include "request.h"

Request::Request(): base_url("http://localhost:8080/") {}

void Request::onProcessRequest(QUrl url) {
    QUrl request_url(url.toString());
    std::cout << request_url.toString().toStdString() << "\n";
    reply.reset();
    network_manager.reset(new QNetworkAccessManager());

    QNetworkRequest request(request_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject user_object;
    user_object["name"] = "Georgiy";
    user_object["password"] = "123";
    QJsonDocument doc(user_object);
    QByteArray body(doc.toJson());
    reply.reset(network_manager->post(request, body));


    connect(reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::httpReadyRead(){
    emit gotHttpData(reply->readAll());
}   
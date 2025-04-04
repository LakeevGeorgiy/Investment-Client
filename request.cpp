#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>

#include "request.h"

Request::Request(): base_url("http://localhost:8080/") {}

void Request::onProcessRequest(QNetworkRequest request, QByteArray body) {
    
    reply.reset();
    network_manager.reset(new QNetworkAccessManager());
    reply.reset(network_manager->post(request, body));


    connect(reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::httpReadyRead(){
    emit gotHttpData(reply->readAll());
}   
#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>

#include "Request.h"

Request::Request() {}

void Request::GetRequest(QNetworkRequest request, QByteArray body) {
    
    reply.reset();
    network_manager.reset(new QNetworkAccessManager());
    reply.reset(network_manager->get(request, body));


    connect(reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::PostRequest(QNetworkRequest request, QByteArray body) {
    
    reply.reset();
    network_manager.reset(new QNetworkAccessManager());
    reply.reset(network_manager->post(request, body));


    connect(reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::httpReadyRead(){
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int code = status_code.toInt();
    qDebug() << "response code: " << code << "\n";
    
    emit gotHttpData(code, reply->readAll());
}   
#include "sender.h"

Sender::Sender(QObject *parent) :
    QObject(parent)

{

    QObject::connect( reply, &QNetworkReply::finished, [this](){

            //QByteArray bytes = reply -> readAll();                                  // bytes

            qDebug() << "Send finished";
            reply->deleteLater();
            man->deleteLater();
        });

    QObject::connect( reply, &QNetworkReply::sslErrors, [this](const QList<QSslError> &errors) {

        qDebug() << "Ssl error" << errors;
        reply->ignoreSslErrors();

    });

}

bool Sender::SendPlease(const char a, const char b, const qint8 c){

    man = new QNetworkAccessManager ();

    auto REQ = QString( "key=pdz9uFCr&id=00006&brand=%1,%2,%3" ).arg( a ).arg( b ).arg( c );

    request.setUrl(( QUrl( QString( "https://kz0.vt-serv.com:500/?" ))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));

    data.append(REQ.toLatin1());

    reply = man -> post(request, data);

    return true;
}
















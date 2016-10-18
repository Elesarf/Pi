#include "sender.h"

Sender::Sender(QObject *parent) :
    QObject(parent)

{

    sz = 0;
    request.setUrl(( QUrl( QString( "https://kz0.vt-serv.com:500/?" ))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));

//    Sender::connect( reply, &QNetworkReply::finished, [this](){

//            QByteArray bytes = reply -> readAll();                                  // bytes

//            qDebug() << "Send finished";

//        });

//    Sender::connect( reply, &QNetworkReply::sslErrors, [this]( const QList<QSslError> &errors ){

//                         qDebug() << "Ssl error" << errors;
//                         reply -> ignoreSslErrors( errors );

//                     });

}

bool Sender::SendPlease(const qint8 a, const qint8 b, const qint8 c){

    if ( sz < 2 ){

            buffer.append( QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c ));
            ++sz;
            qDebug() << QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c );

        } else if( sz == 2 ) {

            buffer.append( QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c ));
            qDebug() << QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c );

            buffer.prepend( "key=pdz9uFCr&id=00006&brand={" );
            buffer.chop( 1 );
            buffer.append( "}" );
            sendStr.append( buffer );

            man = new QNetworkAccessManager ();
            reply = man -> post( request, sendStr );

            Sender::connect( reply, &QNetworkReply::finished, [this](){

                    QByteArray bytes = reply -> readAll();                                  // bytes

                    qDebug() << "Send finished" << bytes;

                    man->deleteLater();
                    reply->deleteLater();

                });

            Sender::connect( reply, &QNetworkReply::sslErrors, [this]( const QList<QSslError> &errors ){

                                 qDebug() << "Ssl error" << errors;
                                 reply -> ignoreSslErrors( errors );

                             });

            qDebug() << "flush buffer" << sendStr;
            sendStr.clear();
            buffer.clear();
            sz = 0;

        }

    return true;
}
















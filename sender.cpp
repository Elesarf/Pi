#include "sender.h"
#include <signal.h>

Sender::Sender(QObject *parent) :
    QObject(parent)
  , man(this)

{
    sz = 0;
    request.setUrl(( QUrl( QString( "https://kz0.vt-serv.com:500/?" ))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));

    Sender::connect( &man, &QNetworkAccessManager::finished, [this](QNetworkReply *reply){

                        reply->deleteLater();

                    });

    Sender::connect( &man, &QNetworkAccessManager::sslErrors, [this](QNetworkReply *reply, const QList<QSslError> &errors ){

                         reply -> ignoreSslErrors( errors );

                     });

}

bool Sender::SendPlease(const qint8 a, const qint8 b, const qint8 c){

    if ( sz < 2 ){

            buffer.append( QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c ));
            ++sz;
            qDebug() << QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c );

        } else {

            buffer.append( QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c ));
            qDebug() << QString( "%1;%2;%3|" ).arg( a - 0x30 ).arg( b ).arg( c );

            buffer.prepend( "key=pdz9uFCr&id=00006&brand={" );
            buffer.chop( 1 );
            buffer.append( "}" );
            sendStr.append( buffer );

            auto reply = man.post( request, sendStr );
            Q_UNUSED( reply );

            qDebug() << "Send to server ";
            sendStr.clear();
            buffer.clear();
            sz = 0;

            return true;
        }

    return false;
}










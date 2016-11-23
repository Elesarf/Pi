#include "sender.h"

Sender::Sender(QObject *parent) :
    QObject(parent)
  , __man( this )
  , __sendTimer( this )

{


    __datetime.addSecs( 3600*3 );
    __sz = 0;
    __timToTimer = 7900;
    __request.setUrl(( QUrl( QString( "https://kz0.vt-serv.com:500/?" ))));
    __request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));
    __sendTimer.setSingleShot( true );

    Sender::connect( &__sendTimer, &QTimer::timeout, [this](){

            qDebug() << "Send on timer";
            Sender::SendPlease(48,77,77,77, 48);

        });

    Sender::connect( &__man, &QNetworkAccessManager::finished, [this](QNetworkReply *reply){

                        reply->deleteLater();

                    });

    Sender::connect( &__man, &QNetworkAccessManager::sslErrors, [this](QNetworkReply *reply, const QList<QSslError> &errors ){

                         reply -> ignoreSslErrors( errors );

                     });

    __sendTimer.start(__timToTimer);

}

bool Sender::SendPlease(const qint8 a, const qint8 b, const qint8 c, int size, qint8 matches){

    __sendTimer.stop();

    QString magicString = QString( "%1;%2;%3;%4%5%6%7;%8|" )
            .arg( a - 0x30 ).arg( b ).arg( c )
            .arg(__datetime.currentDateTime().toString( "yy" ))
            .arg(__datetime.currentDateTime().toString( "MM" ))
            .arg(__datetime.currentDateTime().toString( "dd" ))
            .arg(__datetime.currentDateTime().toString( "HHmmss" ))
            .arg( matches );

    if ( !(( b == 7 ) && ( c == 7 ))){

            if ( __sz < 15 ){

                    __buffer.append( magicString );
                    ++__sz;
                    qDebug() <<  magicString;

                } else {

                    __buffer.append( magicString );
                    qDebug() <<  magicString;

                    __buffer.prepend( "key=pdz9uFCr&id=00006&brand=" );
                    __buffer.chop( 1 );
                    __sendStr.append( __buffer );

                    auto reply = __man.post( __request, __sendStr );
                    Q_UNUSED( reply );

                    qDebug() << "Send to server ";
                    __sendStr.clear();
                    __buffer.clear();
                    __sz = 0;
                    __sendTimer.start(__timToTimer);

                    return true;
                }
        } else {

            __buffer.append( magicString );

            if ( __buffer.size() > 1023 ){

                    __sendStr.clear();
                    __buffer.clear();
                    __sz = 0;
                    __sendTimer.start(__timToTimer);

                }
        }

    __sendTimer.start(__timToTimer);
    return false;
}

bool Sender::IGetImage(const int size, const char place, const char cam){

    __buffer.append(QString( "%1;%2;%3;%4%5%6%7|" )
                    .arg( 0 )
                    .arg( cam - 0x30 )
                    .arg( size )
                    .arg(__datetime.currentDateTime().toString( "yy" ))
                    .arg(__datetime.currentDateTime().toString( "MM" ))
                    .arg(__datetime.currentDateTime().toString( "dd" ))
                    .arg(__datetime.currentDateTime().toString( "HHmmss" )));

    return true;
}







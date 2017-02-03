#include "sender.h"

#include <QNetworkReply>
#include <QDebug>

Sender::Sender(QObject *parent) :
    QObject(parent)
  , __man( this )
  , __sendTimer( this )

{

    __datetime = __datetime.addSecs( 3600*3 ); // local time
    __sz = 0;
    __timToTimer = 7900;
    __FillDispenser();
    __request.setUrl(( QUrl( QString( "https://kz0.vt-serv.com:500/?" ))));
    __request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/x-www-form-urlencoded"));
    __sendTimer.setSingleShot( true );

	system( "sudo echo 4 > /sys/class/gpio/export && echo out > /sys/class/gpio/gpio4/direction" );  //gpio set on pin 4
	system( "sudo echo 1 > /sys/class/gpio/gpio4/value" );

    Sender::connect( &__sendTimer, &QTimer::timeout, [this](){

            qDebug() << "Send on timer";
            Sender::IGetImage( 0, 48, 48 );

        });

    Sender::connect( &__man, &QNetworkAccessManager::finished, [this](QNetworkReply *reply){

                        reply->deleteLater();

                    });

    Sender::connect( &__man, &QNetworkAccessManager::sslErrors, [this](QNetworkReply *reply, const QList<QSslError> &errors ){

                         reply -> ignoreSslErrors( errors );

                     });

    __sendTimer.start(__timToTimer);

}

bool Sender::SendPlease( const qint8 place, const qint8 pack, const qint32 find, int size, qint8 matches ){
    Q_UNUSED (size);

    __sendTimer.stop();

    __dispenser[place - 0x30][pack].place = place - 0x30;
    __dispenser[place - 0x30][pack].pack = pack;
    __dispenser[place - 0x30][pack].timestamp = QString( __datetime.currentDateTime().toString( "yyMMddHHmmss" ));

    if(( __dispenser[place - 0x30][pack].find == find ) || ( __dispenser[place - 0x30][pack].find == -1 )){

            __dispenser[place - 0x30][pack].matches = __dispenser[place - 0x30][pack].matches + matches;
            __dispenser[place - 0x30][pack].find = find ;

        } else if( !( __dispenser[place - 0x30][pack].find == find )){

            if( __dispenser[place - 0x30][pack].matches >= matches ) {

                        __dispenser[place - 0x30][pack].matches = __dispenser[place - 0x30][pack].matches - matches;

                    } else {

                        __dispenser[place - 0x30][pack].find = find ;
                        __dispenser[place - 0x30][pack].matches = matches;

                    }
        }

    if ( find == 0 ){

            __dispenser[place - 0x30][pack].matches = __dispenser[place - 0x30][pack].matches - 2;

        }


    QString magicString;

    for ( qint8 packs = 1; packs < 13; ++ packs ){

            if( __dispenser[1][packs].matches > 9 ){ // 9 find points

                    __dispenser[1][packs].matches = 9;

                }

            magicString.append( QString( "%1;%2;%3;%4;%5|" )

                    .arg( __dispenser[1][packs].place )
                    .arg( packs )
                    .arg( __dispenser[1][packs].find )
                    .arg( __dispenser[1][packs].timestamp )
                    .arg( __dispenser[1][packs].matches ));

        }

    if ( __sz < 12 ){

            ++__sz;
            qDebug() <<  magicString;

        } else {

            __buffer.append( magicString );
            qDebug() <<  magicString;

			__buffer.chop( 1 );
			system("sudo echo 0 > /sys/class/gpio/gpio4/value && echo 'zuz"
			       + __buffer
			       + "\n' > /dev/ttyAMA0 && echo 1 > /sys/class/gpio/gpio4/value");  // UART

            __buffer.prepend( "key=pdz9uFCr&id=00006&brand=" );
            __sendStr.append( __buffer );


            auto reply = __man.post( __request, __sendStr );
            Q_UNUSED( reply );

            qDebug() << "Send to server ";
            __sendStr.clear();
            __buffer.clear();
            __sz = 0;
            __sendTimer.start( __timToTimer );

            return true;
        }

    __sendTimer.start( __timToTimer );
    return false;
}

bool Sender::IGetImage(const int size, const char place, const char cam){
    Q_UNUSED ( place );

    __buffer.append(QString( "%1;%2;%3;%4|" )
                    .arg( 0 )
                    .arg( cam - 0x30 )
                    .arg( size )
                    .arg(__datetime.currentDateTime().toString( "yyMMddHHmmss" )));

    return true;
}

void Sender::__FillDispenser(){

    for( qint8 places = 0; places < 13; ++places ){
            for( qint8 packs = 0; packs < 13; ++packs){

                    __dispenser[places][packs].place        = 0;
                    __dispenser[places][packs].pack         = 0;
                    __dispenser[places][packs].find         = -1;
                    __dispenser[places][packs].matches      = -1;

                }
        }
}































